#ifndef STREAMER_H_
#define STREAMER_H_

#include "video_codec/video_codec_define.h"

#include <string>

namespace nvr
{
class Streamer
{
public:
    virtual ~Streamer() {}

    virtual int32_t Initialize(const std::string &url,
                               int32_t width,
                               int32_t height,
                               int32_t frame_rate,
                               const std::string &sps,
                               const std::string &pps) { return 0; }

    virtual int32_t Initialize(const std::string &url,
                               int32_t width,
                               int32_t height,
                               int32_t frame_rate,
                               const std::string &sps,
                               const std::string &pps,
                               VideoCodecType codec_type) { return 0; }

    virtual int32_t Initialize(const std::string &url,
                               int32_t width,
                               int32_t height,
                               int32_t frame_rate,
                               VideoCodecType codec_type) { return 0; }

    virtual void Close() = 0;

    virtual int32_t WriteVideoFrame(const VideoFrame &frame) = 0;
};
} // namespace nvr

#endif