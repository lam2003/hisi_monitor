#ifndef VIDEO_CODEC_IMPL_H_
#define VIDEO_CODEC_IMPL_H_

#include "video_detect/video_detect.h"

#include <memory>
#include <thread>
#include <vector>

namespace nvr
{

class VideoDetectImpl : public VideoDetectModule
{
public:
    static rtc::scoped_refptr<VideoDetectModule> Create(const Params &params);

    int32_t Initialize(const Params &params) override;

    void Close() override;

protected:
    VideoDetectImpl();

    ~VideoDetectImpl() override;

private:
    int32_t StartMD(const Params &params);

private:
    bool run_;
    std::unique_ptr<std::thread> thread_;
    bool init_;
};
}; // namespace nvr

#endif