#ifndef RECORD_MODULE_H_
#define RECORD_MODULE_H_

#include "base/ref_count.h"
#include "base/scoped_refptr.h"
#include "video_codec/video_codec_define.h"
#include "video/video_sink_interface.h"

#include <string>

namespace nvr
{
class RecordModule : public rtc::RefCountInterface, public VideoSinkInterface<VideoFrame>
{
public:
    struct Params
    {
        int frame_rate;
        int width;
        int height;
        VideoCodecType codec_type;
        std::string filename;
    };
    virtual int32_t Initialize(const Params &params) = 0;

    virtual void Close() = 0;

    virtual void OnFrame(const VideoFrame &) override = 0;

protected:
    virtual ~RecordModule() override = default;
};
} // namespace nvr

#endif
