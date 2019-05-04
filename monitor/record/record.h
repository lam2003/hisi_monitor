#ifndef RECORD_MODULE_H_
#define RECORD_MODULE_H_

#include "video_codec/video_codec_define.h"
#include "video/video_sink_interface.h"
#include "video_detect/video_detect.h"

#include <string>

#include <base/ref_count.h>
#include <base/scoped_refptr.h>

namespace nvr
{
class RecordModule : public rtc::RefCountInterface, public VideoSinkInterface<VideoFrame>, public DetectListener
{
public:
    struct Params
    {
        int frame_rate;
        int width;
        int height;
        std::string path;
        int segment_duration;
        bool use_md;
        int md_duration;
    };
    virtual int32_t Initialize(const Params &params) = 0;

    virtual void Close() = 0;

    virtual void OnFrame(const VideoFrame &) override = 0;

    virtual void OnTrigger(int32_t num) override = 0;

protected:
    virtual ~RecordModule() override = default;
};
} // namespace nvr

#endif
