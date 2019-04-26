#include "live/rtmp_streamer.h"
#include "common/res_code.h"
#include "video_codec/video_codec_define.h"

namespace nvr
{

RTMPStreamer::RTMPStreamer() : fmt_ctx_(nullptr),
                               frame_index_(0),
                               duration_(0),
                               init_(false)
{
}

RTMPStreamer::~RTMPStreamer()
{
}

int32_t RTMPStreamer::Initialize(const std::string &url,
                                 int32_t width,
                                 int32_t height,
                                 int32_t frame_rate,
                                 const std::string &sps,
                                 const std::string &pps)
{
    if (init_)
        return static_cast<int32_t>(KDupInitialize);

    int32_t ret;

    av_register_all();

    avformat_network_init();

    fmt_ctx_ = avformat_alloc_context();
    if (!fmt_ctx_)
    {
        log_e("avformat_alloc_context failed");
        return static_cast<int>(KThirdPartyError);
    }

    strcpy(fmt_ctx_->filename, url.c_str());

    fmt_ctx_->oformat = av_guess_format("flv", url.c_str(), nullptr);
    if (!fmt_ctx_->oformat)
    {
        log_e("av_guess_format failed");
        return static_cast<int>(KThirdPartyError);
    }

    AVStream *stream = avformat_new_stream(fmt_ctx_, nullptr);
    if (!stream)
    {
        log_e("avformat_new_stream failed");
        return static_cast<int>(KThirdPartyError);
    }

    stream->id = 0;
    stream->index = 0;
    stream->time_base = {1, 1000};
    stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    stream->codecpar->codec_id = AV_CODEC_ID_H264;
    stream->codecpar->codec_tag = 0;
    stream->codecpar->width = width;
    stream->codecpar->height = height;
    stream->codecpar->extradata = (uint8_t *)av_malloc(AV_INPUT_BUFFER_PADDING_SIZE + sps.length() + pps.length());
    stream->codecpar->extradata_size = sps.length() + pps.length();
    memset(stream->codecpar->extradata, 0, AV_INPUT_BUFFER_PADDING_SIZE + sps.length() + pps.length());
    memcpy(stream->codecpar->extradata, sps.c_str(), sps.length());
    memcpy(stream->codecpar->extradata + sps.length(), pps.c_str(), pps.length());
    if (fmt_ctx_->oformat->flags & AVFMT_GLOBALHEADER)
        stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    fmt_ctx_->streams[0] = stream;

    av_dump_format(fmt_ctx_, 0, url.c_str(), 1);

    ret = avio_open(&fmt_ctx_->pb, url.c_str(), AVIO_FLAG_WRITE);
    if (ret != 0)
    {
        log_e("avio_open failed");
        return static_cast<int>(KThirdPartyError);
    }

    ret = avformat_write_header(fmt_ctx_, 0);
    if (ret != 0)
    {
        log_e("avformat_write_header failed");
        return static_cast<int>(KThirdPartyError);
    }

    duration_ = (double)AV_TIME_BASE / (double)frame_rate;
    duration_ /= (double)(av_q2d(stream->time_base) * AV_TIME_BASE);

    init_ = true;

    return static_cast<int32_t>(KSuccess);
}

int32_t RTMPStreamer::WriteVideoFrame(const VideoFrame &frame)
{
    if (!init_)
        return static_cast<int>(KUnInitialize);

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = frame.data;
    pkt.size = frame.len;
    pkt.pts = duration_ * frame_index_++;
    pkt.dts = pkt.pts;
    pkt.duration = duration_;
    av_interleaved_write_frame(fmt_ctx_, &pkt);

    return static_cast<int32_t>(KSuccess);
}

void RTMPStreamer::Close()
{
    if (!init_)
        return;

    av_write_trailer(fmt_ctx_);
    //free extradata
    avcodec_parameters_free(&fmt_ctx_->streams[0]->codecpar);
    avformat_free_context(fmt_ctx_);
    fmt_ctx_ = nullptr;
    frame_index_ = 0;
    duration_ = 0;
    init_ = false;
}
}; // namespace nvr