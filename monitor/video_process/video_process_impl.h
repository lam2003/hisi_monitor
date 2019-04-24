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

    void close() override;

protected:
    VideoProcessImpl();

    ~VideoProcessImpl() override;

private:
    int32_t StartVPSSGroup(const Params &params);

    int32_t StartVPSSChn(const Params &params);

private:
    int32_t vpss_chn_fd_;
    bool init_;
};
} // namespace nvr

#endif