#ifndef VIDEO_DETECT_IMPL_H_
#define VIDEO_DETECT_IMPL_H_

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

    void OnFrame(const VIDEO_FRAME_INFO_S &frame) override;

    void AddListener(DetectListener *listener) override;

protected:
    VideoDetectImpl();

    ~VideoDetectImpl() override;

private:
    int32_t StartMD(const Params &params);

    void StopMD();

    int32_t AllocMemory(const Params &params);

    void FreeMemory();

    int32_t IVEDMAImage(const VIDEO_FRAME_INFO_S &frame_info, const IVE_DST_IMAGE_S &dst_image, HI_BOOL instant);

private:
    IVE_SRC_IMAGE_S src_image_[2];
    IVE_DST_MEM_INFO_S dst_mem_info_;
    int32_t trigger_thresh_;
    DetectListener *listener_;
    bool first_frame_;
    int index_;
    bool init_;
};
}; // namespace nvr

#endif