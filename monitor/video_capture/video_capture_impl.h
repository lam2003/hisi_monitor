#ifndef VIDEO_CAPTURE_IMPL_H_
#define VIDEO_CAPTURE_IMPL_H_

#include "video_capture/video_capture.h"

#include <thread>
#include <memory>
#include <mutex>

namespace nvr
{
class VideoCaptureImpl : public VideoCaptureModule
{
public:
    static rtc::scoped_refptr<VideoCaptureModule>
    Create();

    void RegisterCaptureDataCallback(
        VideoSinkInterface<VideoFrame> *dataCallback) override;

    void DeRegisterCaptureDataCallback() override;

    int32_t StartCapture() override;

    int32_t StopCapture() override;

    bool CaptureStarted() override;

    int32_t Initialize();

    void close();

protected:
    int32_t StartMIPI();

    int32_t StartISP();

    VideoCaptureImpl();
    ~VideoCaptureImpl() override;

private:
    std::mutex mux_;
    std::unique_ptr<std::thread> thread_;
    bool run_;
    VideoSinkInterface<VideoFrame> *video_sink_;
    bool init_;
};
} // namespace nvr

#endif