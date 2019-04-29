#include "record/mp4_muxer.h"
#include "common/res_code.h"

#define MP4_RECORDER_BUFFER_SIZE 131072 //128k

namespace nvr
{

MP4Muxer::MP4Muxer() : vps_(""),
                       sps_(""),
                       pps_(""),
                       sei_(""),
                       fmt_ctx_(nullptr),
                       frame_index_(0),
                       buf_(nullptr),
                       init_(false)
{
}

MP4Muxer::~MP4Muxer()
{
    Close();
}

int32_t MP4Muxer::Initialize(const std::string &filename,
                             int32_t width,
                             int32_t height,
                             int32_t frame_rate,
                             VideoCodecType codec_type)
{
    if (init_)
        return static_cast<int32_t>(KDupInitialize);

    int32_t ret;

    codec_type_ = codec_type;

    buf_ = (uint8_t *)av_malloc(MP4_RECORDER_BUFFER_SIZE);
    if (!buf_)
    {
        log_e("av_malloc record buffer failed");
        return static_cast<int>(KThirdPartyError);
    }

    fmt_ctx_ = avformat_alloc_context();
    if (!fmt_ctx_)
    {
        log_e("avformat_alloc_context failed");
        return static_cast<int>(KThirdPartyError);
    }

    strcpy(fmt_ctx_->filename, filename.c_str());

    fmt_ctx_->oformat = av_guess_format(nullptr, filename.c_str(), nullptr);
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
    stream->time_base = {1, frame_rate};
    stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    stream->codecpar->codec_id = (codec_type == H264 ? AV_CODEC_ID_H264 : AV_CODEC_ID_HEVC);
    stream->codecpar->width = width;
    stream->codecpar->height = height;
    if (fmt_ctx_->oformat->flags & AVFMT_GLOBALHEADER)
        stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    fmt_ctx_->streams[0] = stream;

    av_dump_format(fmt_ctx_, 0, filename.c_str(), 1);

    ret = avio_open(&fmt_ctx_->pb, filename.c_str(), AVIO_FLAG_WRITE);
    if (ret != 0)
    {
        log_e("avio_open failed");
        return static_cast<int>(KThirdPartyError);
    }

    ret = avformat_write_header(fmt_ctx_, nullptr);
    if (ret != 0)
    {
        log_e("avformat_write_header failed");
        return static_cast<int>(KThirdPartyError);
    }

    init_ = true;

    return static_cast<int>(KSuccess);
}

int32_t MP4Muxer::WriteVideoFrame(const VideoFrame &frame)
{
    if (!init_)
        return static_cast<int>(KUnInitialize);

    if (codec_type_ == H264)
    {
        return static_cast<int>(WriteH264Frame(frame));
    }
    else if (codec_type_ == H265)
    {
        return static_cast<int>(WriteH265Frame(frame));
    }

    return static_cast<int>(KSuccess);
}

int32_t MP4Muxer::WriteH264Frame(const VideoFrame &frame)
{
    int32_t ret;

    if (frame.type == H264Frame::NaluType::SPS)
    {
        sps_ = std::string((char *)frame.data, frame.len);
        return static_cast<int>(KSuccess);
    }
    else if (frame.type == H264Frame::NaluType::PPS)
    {
        pps_ = std::string((char *)frame.data, frame.len);
        return static_cast<int>(KSuccess);
    }
    else if (frame.type == H264Frame::NaluType::SEI)
    {
        sei_ = std::string((char *)frame.data, frame.len);
        return static_cast<int>(KSuccess);
    }

    if (sps_ == "" || pps_ == "" || sei_ == "")
        return static_cast<int>(KSuccess);

    AVPacket pkt;
    av_init_packet(&pkt);

    if (frame.type == H264Frame::NaluType::ISLICE)
    {
        uint32_t pos = 0;
        memcpy(buf_, sps_.c_str(), sps_.length());
        pos += sps_.length();
        memcpy(buf_ + pos, pps_.c_str(), pps_.length());
        pos += pps_.length();
        memcpy(buf_ + pos, sei_.c_str(), sei_.length());
        pos += sei_.length();
        memcpy(buf_ + pos, frame.data, frame.len);
        pos += frame.len;
        pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.size = pos;
    }
    else
    {
        memcpy(buf_, frame.data, frame.len);
        pkt.size = frame.len;
    }
    pkt.data = buf_;
    pkt.pts = frame_index_++;
    pkt.dts = pkt.pts;

    ret = av_interleaved_write_frame(fmt_ctx_, &pkt);
    if (ret != 0)
    {
        av_free_packet(&pkt);
        log_e("av_interleaved_write_frame failed,code %d", ret);
        PRINT_FFMPEG_ERROR(ret);
        return static_cast<int>(KThirdPartyError);
    }

    av_free_packet(&pkt);

    return static_cast<int>(KSuccess);
}

int32_t MP4Muxer::WriteH265Frame(const VideoFrame &frame)
{
    int32_t ret;
    if (frame.type == H265Frame::NaluType::VPS)
    {
        vps_ = std::string((char *)frame.data, frame.len);
        return static_cast<int>(KSuccess);
    }
    else if (frame.type == H265Frame::NaluType::SPS)
    {
        sps_ = std::string((char *)frame.data, frame.len);
        return static_cast<int>(KSuccess);
    }
    else if (frame.type == H265Frame::NaluType::PPS)
    {
        pps_ = std::string((char *)frame.data, frame.len);
        return static_cast<int>(KSuccess);
    }
    else if (frame.type == H265Frame::NaluType::SEI)
    {
        sei_ = std::string((char *)frame.data, frame.len);
        return static_cast<int>(KSuccess);
    }

    if (vps_ == "" || sps_ == "" || pps_ == "" || sei_ == "")
        return static_cast<int>(KSuccess);

    AVPacket pkt;
    av_init_packet(&pkt);

    if (frame.type == H265Frame::NaluType::IDRSLICE)
    {
        uint32_t pos = 0;
        memcpy(buf_, vps_.c_str(), vps_.length());
        pos += vps_.length();
        memcpy(buf_ + pos, sps_.c_str(), sps_.length());
        pos += sps_.length();
        memcpy(buf_ + pos, pps_.c_str(), pps_.length());
        pos += pps_.length();
        memcpy(buf_ + pos, sei_.c_str(), sei_.length());
        pos += sei_.length();
        memcpy(buf_ + pos, frame.data, frame.len);
        pos += frame.len;
        pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.size = pos;
    }
    else
    {
        memcpy(buf_, frame.data, frame.len);
        pkt.size = frame.len;
    }
    pkt.data = buf_;
    pkt.pts = frame_index_++;
    pkt.dts = pkt.pts;

    ret = av_interleaved_write_frame(fmt_ctx_, &pkt);
    if (ret != 0)
    {
        av_free_packet(&pkt);
        log_e("av_interleaved_write_frame failed,code %d", ret);
        PRINT_FFMPEG_ERROR(ret);
        return static_cast<int>(KThirdPartyError);
    }

    av_free_packet(&pkt);

    return static_cast<int>(KSuccess);
}

void MP4Muxer::Close()
{
    if (!init_)
        return;

    av_write_trailer(fmt_ctx_);
    avcodec_parameters_free(&fmt_ctx_->streams[0]->codecpar);
    avio_close(fmt_ctx_->pb);
    avformat_free_context(fmt_ctx_);
    av_free(buf_);

    sps_ = "";
    pps_ = "";
    sei_ = "";
    fmt_ctx_ = nullptr;
    frame_index_ = 0;
    buf_ = nullptr;
    init_ = false;
}
}; // namespace nvr