#include "video_capture/video_capture_impl.h"
#include "base/ref_counted_object.h"
#include "common/res_code.h"

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

void VideoCaptureImpl::DeRegisterCaptureDataCallback()
{
    video_sink_ = nullptr;
}

int32_t VideoCaptureImpl::Initialize(const VideoCaptureCapability &capability)
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    return static_cast<int>(KSuccess);
}

VideoCaptureImpl::VideoCaptureImpl() : thread_(nullptr),
                                       run_(false),
                                       video_sink_(nullptr),
                                       init_(false)
{
}

VideoCaptureImpl::~VideoCaptureImpl()
{
    close();
}

}; // namespace nvr