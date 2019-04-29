#ifndef VIDEO_DETECT_MODULE_H_
#define VIDEO_DETECT_MODULE_H_

#include "base/scoped_refptr.h"
#include "base/ref_count.h"
#include "video/video_sink_interface.h"
#include "video/video_frame.h"

namespace nvr
{
class DetectListener
{
public:
    virtual ~DetectListener() {}
    virtual void OnTrigger(int32_t num) = 0;
};

class VideoDetectModule : public rtc::RefCountInterface, public VideoSinkInterface<VIDEO_FRAME_INFO_S>
{
public:
    struct Params
    {
        int32_t width;
        int32_t height;
        int32_t trigger_thresh;
    };

    virtual int32_t Initialize(const Params &params) = 0;

    virtual void Close() = 0;

    virtual void OnFrame(const VIDEO_FRAME_INFO_S &frame) = 0;

    virtual void AddListener(DetectListener *listener) = 0;

protected:
    ~VideoDetectModule() override {}
};
}; // namespace nvr

#endif