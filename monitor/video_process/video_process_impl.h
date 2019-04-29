#ifndef VIDEO_PROCESS_IMPL_H_
#define VIDEO_PROCESS_IMPL_H_

#include "video_process/video_process.h"

#include <memory>
#include <thread>
#include <mutex>

namespace nvr
{
class VideoProcessImpl : public VideoProcessModule
{
public:
    static rtc::scoped_refptr<VideoProcessModule> Create(const Params &params);

    int32_t Initialize(const Params &params) override;

    void Close() override;

    void SetVideoSink(VideoSinkInterface<VIDEO_FRAME_INFO_S> *video_sink) override;

protected:
    VideoProcessImpl();

    ~VideoProcessImpl() override;

private:
    int32_t StartVPSSGroup(const Params &params);

    void StopVPSSGroup();

    int32_t StartVPSSEncodeChn(const Params &params);

    void StopVPSSEncodeChn();

    int32_t StartVPSSDetectChn(const Params &params);

    void StopVPSSDetectChn();

    void StartProcessThread(const Params &params);

    void StopProcessThread();

private:
    std::mutex mux_;
    bool run_;
    std::unique_ptr<std::thread> thread_;
    VideoSinkInterface<VIDEO_FRAME_INFO_S> *video_sink_;
    bool init_;
};
} // namespace nvr

#endif