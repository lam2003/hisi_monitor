#ifndef LIVE_MODULE_H_
#define LIVE_MODULE_H_

#include "video_codec/video_codec_define.h"
#include "video/video_sink_interface.h"

#include <base/ref_count.h>
#include <base/scoped_refptr.h>

#include <string>

namespace nvr
{
class LiveModule : public rtc::RefCountInterface, public VideoSinkInterface<VideoFrame>
{
public:
    struct Params
    {
        std::string url;
    };

    virtual int32_t Initialize(const Params &params) = 0;

    virtual void Close() = 0;

    virtual void OnFrame(const VideoFrame &) override = 0;

protected:
    ~LiveModule() override = default;
};
}; // namespace nvr

#endif