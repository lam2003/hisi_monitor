#ifndef VIDEO_DETECT_MODULE_H_
#define VIDEO_DETECT_MODULE_H_

#include "base/scoped_refptr.h"
#include "base/ref_count.h"
#include "video/video_sink_interface.h"

namespace nvr
{

class VideoDetectModule : public rtc::RefCountInterface
{
public:
    struct Params
    {
        int32_t width;
        int32_t height;
        int32_t frame_rate;
    };

    virtual int32_t Initialize(const Params &params) = 0;

    virtual void Close() = 0;

protected:
    ~VideoDetectModule() override {}
};
}; // namespace nvr

#endif