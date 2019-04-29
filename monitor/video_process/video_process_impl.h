#ifndef VIDEO_PROCESS_IMPL_H_
#define VIDEO_PROCESS_IMPL_H_

#include <memory>
#include <thread>

#include "video_process/video_process.h"

namespace nvr
{
class VideoProcessImpl : public VideoProcessModule
{
public:
    static rtc::scoped_refptr<VideoProcessModule> Create(const Params &params);

    int32_t Initialize(const Params &params) override;

    void Close() override;

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

private:
    bool init_;
};
} // namespace nvr

#endif