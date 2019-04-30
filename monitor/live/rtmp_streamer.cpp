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
