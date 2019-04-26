#ifndef RTMP_STREAM_H_
#define RTMP_STREAM_H_

#include "live/streamer.h"

extern "C"
{
#include "libavformat/avformat.h"
}

namespace nvr
{
//only support h264

class RTMPStreamer : public Streamer
{
public:
    RTMPStreamer();

    ~RTMPStreamer() override;

    int32_t Initialize(const std::string &url,
                       int32_t width,
                       int32_t height,
                       int32_t frame_rate,
                       const std::string &sps,
                       const std::string &pps);

    void Close();

    int32_t WriteVideoFrame(const VideoFrame &frame);

private:
    AVFormatContext *fmt_ctx_;
    uint64_t frame_index_;
    uint64_t duration_;
    bool init_;
};
}; // namespace nvr

#endif