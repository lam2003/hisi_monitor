#ifndef VIDEO_PROCESS_MODULE_H_
#define VIDEO_PROCESS_MODULE_H_

#include "base/scoped_refptr.h"
#include "base/ref_count.h"
#include "video/video_sink_interface.h"

namespace nvr
{
class VideoProcessModule : public rtc::RefCountInterface
{
public:
    struct Params
    {
        int32_t frame_rate;
        int32_t encode_width;
        int32_t encode_height;
        int32_t detect_width;
        int32_t detect_height;
    };

    virtual int32_t Initialize(const Params &params) = 0;

    virtual void Close() = 0;

    virtual void SetVideoSink(VideoSinkInterface<VIDEO_FRAME_INFO_S> *video_sink) = 0;

protected:
    ~VideoProcessModule() override{};
};
}; // namespace nvr

#endif