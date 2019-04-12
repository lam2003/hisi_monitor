#ifndef VIDEO_CAPTURE_IMPL_H_
#define VIDEO_CAPTURE_IMPL_H_

#include "video_capture/video_capture.h"
#include "video_capture/video_capture_capability.h" 

#include <thread>
#include <memory>
#include <mutex>

namespace nvr
{
class VideoCaptureImpl : public VideoCaptureModule
{
public:
    static rtc::scoped_refptr<VideoCaptureModule>
    Create(const VideoCaptureCapability &capability);

    void RegisterCaptureDataCallback(
        VideoSinkInterface<VideoFrame> *dataCallback) override;

    void DeRegisterCaptureDataCallback() override;

    int32_t StartCapture() override;

    int32_t StopCapture() override;

    bool CaptureStarted() override;

    int32_t Initialize(const VideoCaptureCapability &capability);

    void close();

protected:
    VideoCaptureImpl();
    ~VideoCaptureImpl() override;

private:
    VideoSinkInterface<VideoFrame> *video_sink_;
    bool run_;
    std::unique_ptr<std::thread> thread_;
    std::mutex mux_;

    bool init_;
};
} // namespace nvr

#endif