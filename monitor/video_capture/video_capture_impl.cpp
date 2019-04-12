#include "video_capture/video_capture_impl.h"
#include "base/ref_counted_object.h"

namespace nvr
{
rtc::scoped_refptr<VideoCaptureModule> VideoCaptureImpl::Create(const VideoCaptureCapability &capability)
{
    rtc::scoped_refptr<VideoCaptureImpl> implemention(new rtc::RefCountedObject<VideoCaptureImpl>());
    if (implemention->Initialize(capability) != 0)
        return nullptr;
    return implemention;
}

void VideoCaptureImpl::RegisterCaptureDataCallback(VideoSinkInterface<VideoFrame> *video_sink)
{
    video_sink_ = video_sink;
}


}; // namespace nvr