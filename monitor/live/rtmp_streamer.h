#ifndef RTMP_STREAM_H_
#define RTMP_STREAM_H_

#include "live/streamer.h"

#include <srs_librtmp.h>

namespace nvr
{
class RTMPStreamer : public Streamer
{
public:
    RTMPStreamer();

    ~RTMPStreamer() override;

    int32_t Initialize(const std::string &u) override;

    void Close() override;

    int32_t WriteVideoFrame(const VideoFrame &frame) override;

private:
    srs_rtmp_t rtmp_;
    bool init_;
};
}; // namespace nvr

#endif