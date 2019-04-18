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
    static rtc::scoped_refptr<VideoProcessModule> Create();

    int32_t SetRotate(ROTATE_E rotate) override;

    int32_t SetFrameRate(int frame_rate) override;

    int32_t SetMirror(bool mirror) override;

    int32_t SetFlip(bool flip) override;

    int32_t setScale(const SIZE_S &size) override;

    int32_t Initialize() override;

    void close() override;

protected:
    VideoProcessImpl();

    ~VideoProcessImpl() override;

private:
    int32_t StartVPSSGroup();

    int32_t StartVPSSChn();

private:
    int32_t vpss_chn_fd_;
    bool init_;
};
} // namespace nvr

#endif