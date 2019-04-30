// #include "live/rtmp_streamer.h"
// #include "common/res_code.h"
// #include "video_codec/video_codec_define.h"

// #define RTMP_STREAMER_BUFFER_SIZE 131072 //128k

// namespace nvr
// {

// RTMPStreamer::RTMPStreamer() : fmt_ctx_(nullptr),
//                                frame_index_(0),
//                                duration_(0),
//                                init_(false)
// {
// }

// RTMPStreamer::~RTMPStreamer()
// {
//     Close();
// }

// int32_t RTMPStreamer::Initialize(const std::string &url,
//                                  int32_t width,
//                                  int32_t height,
//                                  int32_t frame_rate,
//                                  const std::string &sps,
//                                  const std::string &pps)
// {
//     if (init_)
//         return static_cast<int32_t>(KDupInitialize);

//     int32_t ret;

//     fmt_ctx_ = avformat_alloc_context();
//     if (!fmt_ctx_)
//     {
//         log_e("avformat_alloc_context failed");
//         return static_cast<int>(KThirdPartyError);
//     }

//     strcpy(fmt_ctx_->filename, url.c_str());

//     fmt_ctx_->oformat = av_guess_format("flv", url.c_str(), nullptr);
//     if (!fmt_ctx_->oformat)
//     {
//         log_e("av_guess_format failed");
//         return static_cast<int>(KThirdPartyError);
//     }

//     AVStream *stream = avformat_new_stream(fmt_ctx_, nullptr);
//     if (!stream)
//     {
//         log_e("avformat_new_stream failed");
//         return static_cast<int>(KThirdPartyError);
//     }

//     stream->id = 0;
//     stream->index = 0;
//     stream->time_base = {1, 1000};
//     stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
//     stream->codecpar->codec_id = AV_CODEC_ID_H264;
//     stream->codecpar->codec_tag = 0;
//     stream->codecpar->width = width;
//     stream->codecpar->height = height;
//     stream->codecpar->extradata = (uint8_t *)av_malloc(AV_INPUT_BUFFER_PADDING_SIZE + sps.length() + pps.length());
//     stream->codecpar->extradata_size = sps.length() + pps.length();
//     memset(stream->codecpar->extradata, 0, AV_INPUT_BUFFER_PADDING_SIZE + sps.length() + pps.length());
//     memcpy(stream->codecpar->extradata, sps.c_str(), sps.length());
//     memcpy(stream->codecpar->extradata + sps.length(), pps.c_str(), pps.length());
//     if (fmt_ctx_->oformat->flags & AVFMT_GLOBALHEADER)
//         stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//     fmt_ctx_->streams[0] = stream;

//     av_dump_format(fmt_ctx_, 0, url.c_str(), 1);

//     ret = avio_open(&fmt_ctx_->pb, url.c_str(), AVIO_FLAG_WRITE);
//     if (ret != 0)
//     {
//         log_e("avio_open failed");
//         return static_cast<int>(KThirdPartyError);
//     }

//     ret = avformat_write_header(fmt_ctx_, nullptr);
//     if (ret != 0)
//     {
//         log_e("avformat_write_header failed");
//         return static_cast<int>(KThirdPartyError);
//     }

//     duration_ = (double)AV_TIME_BASE / (double)frame_rate;
//     duration_ /= (double)(av_q2d(stream->time_base) * AV_TIME_BASE);

//     init_ = true;

//     return static_cast<int>(KSuccess);
// }

// int32_t RTMPStreamer::WriteVideoFrame(const VideoFrame &frame)
// {
//     if (!init_)
//         return static_cast<int>(KUnInitialize);

//     if (frame.type == H264Frame::NaluType::SPS ||
//         frame.type == H264Frame::NaluType::PPS ||
//         frame.type == H264Frame::NaluType::SEI)
//         return static_cast<int>(KSuccess);

//     int32_t ret;
//     AVPacket pkt;
//     av_init_packet(&pkt);

//     pkt.flags = (frame.type == H264Frame::NaluType::ISLICE ? AV_PKT_FLAG_KEY : 0);
//     pkt.data = frame.data;
//     pkt.size = frame.len;
//     pkt.pts = duration_ * frame_index_++;
//     pkt.dts = pkt.pts;
//     pkt.duration = duration_;
//     pkt.pos = -1;

//     ret = av_interleaved_write_frame(fmt_ctx_, &pkt);
//     if (ret != 0)
//     {
//         av_free_packet(&pkt);
//         log_e("av_interleaved_write_frame failed,code %d", ret);
//         PRINT_FFMPEG_ERROR(ret);
//         return static_cast<int>(KThirdPartyError);
//     }
//     av_free_packet(&pkt);

//     return static_cast<int>(KSuccess);
// }

// void RTMPStreamer::Close()
// {
//     if (!init_)
//         return;

//     avcodec_parameters_free(&fmt_ctx_->streams[0]->codecpar);
//     avio_close(fmt_ctx_->pb);
//     avformat_free_context(fmt_ctx_);
//     fmt_ctx_ = nullptr;
//     frame_index_ = 0;
//     duration_ = 0;
//     init_ = false;
// }
// }; // namespace nvr

#include "live/rtmp_streamer.h"
#include "common/res_code.h"

namespace nvr
{

RTMPStreamer::RTMPStreamer() : rtmp_(nullptr),
                               init_(false)
{
}

RTMPStreamer::~RTMPStreamer()
{
    Close();
}

int32_t RTMPStreamer::Initialize(const std::string &url)
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    rtmp_ = srs_rtmp_create(url.c_str());
    if (srs_rtmp_handshake(rtmp_) != 0)
    {
        log_e("srs_rtmp_handshake failed");
        return static_cast<int>(KThirdPartyError);
    }

    if (srs_rtmp_connect_app(rtmp_) != 0)
    {
        log_e("srs_rtmp_connect_app failed");
        return static_cast<int>(KThirdPartyError);
    }

    if (srs_rtmp_publish_stream(rtmp_) != 0)
    {
        log_e("srs_rtmp_publish_stream failed");
        return static_cast<int>(KThirdPartyError);
    }

    init_ = true;

    return static_cast<int>(KSuccess);
}

int32_t RTMPStreamer::WriteVideoFrame(const VideoFrame &frame)
{
#define ERROR_H264_DROP_BEFORE_SPS_PPS 3043
#define ERROR_H264_DUPLICATED_SPS 3044
#define ERROR_H264_DUPLICATED_PPS 3045
    if (!init_)
        return static_cast<int>(KUnInitialize);

    int32_t ret;
    ret = srs_h264_write_raw_frames(rtmp_, (char *)frame.data, frame.len, frame.ts / 1000, frame.ts / 1000);
    if (ret != 0)
    {
        if (ret != ERROR_H264_DROP_BEFORE_SPS_PPS &&
            ret != ERROR_H264_DUPLICATED_SPS &&
            ret != ERROR_H264_DUPLICATED_PPS)
        {
            log_e("srs_h264_write_raw_frames failed,code %d", ret);
            return static_cast<int>(KThirdPartyError);
        }
    }
    return static_cast<int>(KSuccess);
}

void RTMPStreamer::Close()
{
    if (!init_)
        return;
    srs_rtmp_destroy(rtmp_);
    init_ = false;
}

} // namespace nvr
