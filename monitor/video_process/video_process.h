#ifndef VIDEO_PROCESS_MODULE_H_
#define VIDEO_PROCESS_MODULE_H_

#include "video/video_sink_interface.h"
#include "base/scoped_refptr.h"
#include "base/ref_count.h"

namespace nvr
{
class VideoProcessModule : public rtc::RefCountInterface
{
public:
    virtual int32_t SetRotate(ROTATE_E rotate) = 0;

    virtual int32_t SetFrameRate(int frame_rate) = 0;

    virtual int32_t SetMirror(bool mirror) = 0;

    virtual int32_t SetFlip(bool flip) = 0;

    virtual int32_t setScale(const SIZE_S &size) = 0;

    virtual int32_t Initialize() = 0;

    virtual void close() = 0;

protected:
    ~VideoProcessModule() override{};
};
}; // namespace nvr

#endif