#include "live/rtmp.h"
#include "common/res_code.h"
#include "base/ref_counted_object.h"

#define IO_BUFFER_SIZE (32 * 1024)

namespace nvr
{

rtc::scoped_refptr<LiveModule> RtmpLiveImpl::Create(const Params &params)
{
    err_code code;

    rtc::scoped_refptr<RtmpLiveImpl> implemention = new rtc::RefCountedObject<RtmpLiveImpl>();

    code = static_cast<err_code>(implemention->Initialize(params));

    if (KSuccess != code)
    {
        log_e("error:%s", make_error_code(code).message().c_str());
        return nullptr;
    }

    return implemention;
}

int32_t RtmpLiveImpl::ReadBuffer(void *opaque, uint8_t *buf, int requested_len)
{
    RtmpLiveImpl *rtmp_live_impl = static_cast<RtmpLiveImpl *>(opaque);

    bool ret;
    do
    {
        std::unique_lock<std::mutex> lock(rtmp_live_impl->mux_);
        if (ret = rtmp_live_impl->buffer_.get(buf, requested_len))
            rtmp_live_impl->cond_.wait(lock);
    } while (rtmp_live_impl->run_ && !ret);

    if (!rtmp_live_impl->run_)
        return 0;

    return requested_len;
}

int32_t RtmpLiveImpl::Initialize(const Params &params)
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    NVR_CHECK(params.codec_type == H264);

    run_ = true;
    thread_ = std::unique_ptr<std::thread>(new std::thread([this, params]() {
        int32_t ret;

        av_register_all();

        avformat_network_init();

        AVFormatContext *ifmt_ctx;
        ifmt_ctx = avformat_alloc_context();
        if (!ifmt_ctx)
        {
            log_e("avformat_alloc_context failed");
            return;
        }

        uint8_t *in_buffer = (uint8_t *)av_malloc(IO_BUFFER_SIZE);
        if (!in_buffer)
        {
            log_e("malloc io buffer failed");
            return;
        }

        AVIOContext *iavio_ctx = avio_alloc_context(in_buffer, IO_BUFFER_SIZE, 0, this, RtmpLiveImpl::ReadBuffer, nullptr, nullptr);
        if (!iavio_ctx)
        {
            log_e("avio_alloc_context failed");
            return;
        }

        ifmt_ctx->pb = iavio_ctx;
        ifmt_ctx->flags = AVFMT_FLAG_CUSTOM_IO;

        ret = avformat_open_input(&ifmt_ctx, NULL, NULL, NULL);
        if (ret != 0)
        {
            log_e("avformat_open_input failed,code %#x", ret);
            return;
        }

        av_dump_format(ifmt_ctx, 0, "memory", 0);

        AVFormatContext *ofmt_ctx;
        ofmt_ctx = avformat_alloc_context();
        if (!ofmt_ctx)
        {
            log_e("avformat_alloc_context failed");
            return;
        }
        memcpy(ofmt_ctx->filename, params.url.c_str(), params.url.length());

        ofmt_ctx->oformat = av_guess_format("flv", params.url.c_str(), nullptr);

        AVStream *out_stream = avformat_new_stream(ofmt_ctx, ifmt_ctx->streams[0]->codec->codec);
        if (!out_stream)
        {
            log_e("avformat_new_stream failed");
            return;
        }

        ret = avcodec_parameters_copy(out_stream->codecpar, ifmt_ctx->streams[0]->codecpar);
        if (ret != 0)
        {
            log_e("avcodec_parameters_copy failed");
            return;
        }

        out_stream->codecpar->width = params.width;
        out_stream->codecpar->height = params.height;
        out_stream->codecpar->extradata = (uint8_t *)av_malloc(AV_INPUT_BUFFER_PADDING_SIZE + (sps_len_ + pps_len_));
        memset(out_stream->codecpar->extradata, 0, AV_INPUT_BUFFER_PADDING_SIZE + (sps_len_ + pps_len_));
        memcpy(out_stream->codecpar->extradata, sps_, sps_len_);
        memcpy(out_stream->codecpar->extradata + sps_len_, pps_, pps_len_);
        out_stream->codecpar->extradata_size = sps_len_ + pps_len_;

        out_stream->codecpar->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        av_dump_format(ofmt_ctx, 0, params.url.c_str(), 1);

        ret = avio_open(&ofmt_ctx->pb, params.url.c_str(), AVIO_FLAG_WRITE);
        if (ret != 0)
        {
            log_e("avio_open failed");
            return;
        }

        ret = avformat_write_header(ofmt_ctx, 0);
        if (ret != 0)
        {
            log_e("avformat_write_header failed");
            return;
        }

        uint64_t duration = (double)AV_TIME_BASE / (double)params.frame_rate;
        AVRational time_base = ofmt_ctx->streams[0]->time_base;
        duration = (double)duration / (double)(av_q2d(time_base) * AV_TIME_BASE);
        uint64_t frame_index = 0;

        while (run_)
        {
            AVPacket pkt;
            ret = av_read_frame(ifmt_ctx, &pkt);
            if (ret == AVERROR_EOF)
            {
                log_w("av_read_frame got eof...");
                break;
            }
            
            pkt.pts = duration * frame_index;
            pkt.dts = pkt.pts;
            pkt.duration = duration;
            frame_index++;

            ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
            if (ret != 0)
            {
                log_e("av_interleaved_write_frame failed,code %#x", ret);
                break;
            }
        }

        av_write_trailer(ofmt_ctx);

        avio_close(ofmt_ctx->pb);

        avformat_free_context(ofmt_ctx);

        avformat_close_input(&ifmt_ctx);

        av_free(in_buffer);
    }));

    return static_cast<int>(KSuccess);
}

void RtmpLiveImpl::OnFrame(const VideoFrame &frame)
{
    NVR_CHECK(frame.GetCodecType() == H264);

    if (/* !got_sps_pps_ && */ H264Frame::NaluType::SPS == static_cast<H264Frame::NaluType>(frame.type))
    {
        // log_w("waiting sps/pps");
        // return;
        memcpy(sps_, frame.data, frame.len);
        sps_len_ = frame.len;
    }

    if (/* !got_sps_pps_ && */ H264Frame::NaluType::PPS == static_cast<H264Frame::NaluType>(frame.type))
    {
        // log_w("waiting sps/pps");
        // return;
        memcpy(pps_, frame.data, frame.len);
        pps_len_ = frame.len;
    }

    // if (!got_sps_pps_)
    //     got_sps_pps_ = true;

    std::unique_lock<std::mutex>(mux_);
    if (!buffer_.append((uint8_t *)frame.data, frame.len))
    {
        log_w("buffer overflow");
        return;
    }
    cond_.notify_one();
}

void RtmpLiveImpl::Close()
{
    run_ = false;
}

RtmpLiveImpl::RtmpLiveImpl() : run_(false),
                               got_sps_pps_(false),
                               sps_len_(0),
                               pps_len_(0),
                               init_(false)
{
}

RtmpLiveImpl::~RtmpLiveImpl()
{
}
}; // namespace nvr