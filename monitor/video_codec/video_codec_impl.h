#ifndef VIDEO_CODEC_IMPL_H_
#define VIDEO_CODEC_IMPL_H_

#include "video_codec/video_codec.h"

#include <memory>
#include <thread>
#include <vector>

namespace nvr
{

class VideoCodecImpl : public VideoCodecModule
{
public:
  static rtc::scoped_refptr<VideoCodecModule> Create(const Params &params);

  int32_t Initialize(const Params &params) override;

  void Close() override;

  void AddVideoSink(VideoSinkInterface<VideoFrame> *video_sink) override;

protected:
  VideoCodecImpl();

  ~VideoCodecImpl() override;

private:
  int32_t StartVENCChn(const Params &params);

  void StopVENCChn();

  void StartGetStreamThread(const Params &params);

  void StopGetStreamThread();

private:
  bool run_;
  std::unique_ptr<std::thread> thread_;
  std::vector<VideoSinkInterface<VideoFrame> *> video_sinks_;
  bool init_;
};
}; // namespace nvr

#endif