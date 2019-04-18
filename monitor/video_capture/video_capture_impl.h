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
    static rtc::scoped_refptr<VideoCaptureModule> Create();

    int32_t Initialize() override;

    void close() override;

protected:
    ~VideoCaptureImpl() override;

    VideoCaptureImpl();

private:
    int32_t StartMIPI();

    int32_t InitISP();

    void StartISP();

    int32_t StartVI();

    int32_t StartVIChn();

private:
    std::mutex mux_;
    int32_t vi_chn_fd_;
    std::unique_ptr<std::thread> isp_thread_;
    bool init_;
};
} // namespace nvr

#endif