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

    void Close() override;

protected:
    ~VideoCaptureImpl() override;

    VideoCaptureImpl();

private:
    int32_t StartMIPI();

    int32_t InitISP();

    void UnInitISP();

    void StartISP();

    void StopISP();

    int32_t StartVI();

    void StopVI();

    int32_t StartVIChn();

    void StopVIChn();

private:
    std::unique_ptr<std::thread> thread_;
    bool init_;
};
} // namespace nvr

#endif