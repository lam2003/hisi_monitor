#ifndef RTMP_STREAMER_H_2_
#define RTMP_STREAMER_H_2_

#include "live/streamer.h"
#include "video_codec/video_codec_define.h"

#include <librtmp/rtmp.h>

#include <mutex>

//rtmpdump实现,只支持h264

namespace nvr
{
class RTMPStreamer2 : public Streamer
{
public:
    RTMPStreamer2();

    ~RTMPStreamer2() override;

    int32_t Initialize(const std::string &url) override;

    void Close() override;

    int32_t WriteVideoFrame(const VideoFrame &frame) override;

private:
    int32_t SendH264MetaData(const std::string &sps, const std::string &pps);

private:
    std::string sps_;
    std::string pps_;
    bool send_meta_;
    RTMP *rtmp_;
    uint8_t *buf_;
    bool init_;
};
} // namespace nvr

#endif
