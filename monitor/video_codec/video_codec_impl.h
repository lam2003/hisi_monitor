#ifndef VIDEO_CODEC_IMPL_H_
#define VIDEO_CODEC_IMPL_H_

#include "video_codec/video_codec.h"

#include <memory>
#include <thread>

namespace nvr
{

class VideoCodecImpl : public VideoCodecModule
{
  public:
    static rtc::scoped_refptr<VideoCodecModule> Create(const Params &params);

    int32_t Initialize(const Params &params) override;

    void Close() override;

    void SetVideoSinkInterface(VideoSinkInterface<VideoFrame> *video_sink) override;

  protected:
    VideoCodecImpl();

    ~VideoCodecImpl() override;

  private:
    int32_t StartVENCChn(const Params &params);

    void StartGetStreamThread(const Params &params);

  private:
    bool run_;
    std::unique_ptr<std::thread> thread_;
    VideoSinkInterface<VideoFrame> *video_sink_;
    bool init_;
};
}; // namespace nvr

#endif