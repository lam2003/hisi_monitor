#ifndef LIVE_MODULE_H_
#define LIVE_MODULE_H_

#include "base/ref_count.h"
#include "base/scoped_refptr.h"
#include "video_codec/video_codec_define.h"
#include "video/video_sink_interface.h"

namespace nvr
{
class LiveModule : public rtc::RefCountInterface, public VideoSinkInterface<VideoFrame>
{
public:
    struct Params
    {
        int32_t frame_rate;
        int32_t width;
        int32_t height;
        VideoCodecType codec_type;
        std::string url;
    };

    virtual int32_t Initialize(const Params &params) = 0;

    virtual void Close() = 0;

    virtual void OnFrame(const VideoFrame &frame) override = 0;

protected:
    ~LiveModule() override {}
};
}; // namespace nvr

#endif