#ifndef RTMP_STREAMER_H_2_
#define RTMP_STREAMER_H_2_

#include "live/streamer.h"
#include "video_codec/video_codec_define.h"

#include <librtmp/rtmp.h>

#include <mutex>
//rtmpdump实现,支持h264/h265

namespace nvr
{
class RTMPStreamer2 : public Streamer
{
public:
    RTMPStreamer2();

    ~RTMPStreamer2() override;

    int32_t Initialize(const std::string &url,
                       int32_t width,
                       int32_t height,
                       int32_t frame_rate,
                       VideoCodecType codec_type) override;

    void Close() override;

    int32_t WriteVideoFrame(const VideoFrame &frame) override;

protected:
    //rtmpdump在初始化不可重入,需要加锁保证线程同步
    static std::mutex KMux;

private:
    int32_t SendH264MetaData(const std::string &sps, const std::string &pps);
    int32_t SendH264Data(const VideoFrame &frame);
    int32_t SendH265Data(const VideoFrame &frame);

private:
    VideoCodecType codec_type_;
    std::string sps_;
    std::string pps_;
    bool send_meta_;
    RTMP *rtmp_;
    uint8_t *buf_;
    bool init_;
};
} // namespace nvr

#endif
