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
        VideoSinkInterface<VIDEO_FRAME_INFO_S> *dataCallback) override;

    void DeRegisterCaptureDataCallback() override;

    int32_t StartCapture() override;

    int32_t StopCapture() override;

    int32_t Initialize();

    void close();

protected:
    int32_t StartMIPI();

    int32_t InitISP();

    void StartISP();

    int32_t StartVI();

    int32_t StartVIChn();

    VideoCaptureImpl();
    ~VideoCaptureImpl() override;

private:
    std::mutex mux_;
    int32_t vi_chn_fd_;
    std::unique_ptr<std::thread> capture_thread_;
    std::unique_ptr<std::thread> isp_thread_;
    bool run_;
    VideoSinkInterface<VIDEO_FRAME_INFO_S> *video_sink_;
    bool init_;
};
} // namespace nvr

#endif