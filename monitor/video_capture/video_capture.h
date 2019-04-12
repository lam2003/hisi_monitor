#ifndef VIDEO_CAPTRUE_MODULE_H_
#define VIDEO_CAPTURE_MODULE_H_

#include "video/video_sink_interface.h"
#include "video/video_frame.h"
#include "base/scoped_refptr.h"
#include "base/ref_count.h"

namespace nvr
{

class VideoCaptureModule : public rtc::RefCountInterface
{
public:
    virtual void RegisterCaptureDataCallback(
        VideoSinkInterface<VideoFrame> *dataCallback) = 0;

    virtual void DeRegisterCaptureDataCallback() = 0;

    virtual int32_t StartCapture() = 0;

    virtual int32_t StopCapture() = 0;

    virtual bool CaptureStarted() = 0;

protected:
    ~VideoCaptureModule() override {}

};

} // namespace nvr

#endif