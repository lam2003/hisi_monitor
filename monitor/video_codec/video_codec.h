#ifndef VIDEO_CODEC_MODULE_H_
#define VIDEO_CODEC_MODULE_H_

#include "video/video_sink_interface.h"
#include "video_codec/video_codec_define.h"

#include <base/scoped_refptr.h>
#include <base/ref_count.h>

namespace nvr
{

class VideoCodecModule : public rtc::RefCountInterface
{
public:
  struct Params
  {
    int32_t frame_rate;
    int32_t width;
    int32_t height;
    VideoCodecMode codec_mode;
    int32_t profile;
    int32_t bitrate;
  };

  virtual int32_t Initialize(const Params &params) = 0;

  virtual void Close() = 0;

  virtual void AddVideoSink(VideoSinkInterface<VideoFrame> *video_sink) = 0;

  virtual void ClearVideoSink() = 0;

protected:
  ~VideoCodecModule() override {}
};
}; // namespace nvr

#endif