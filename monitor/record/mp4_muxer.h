#ifndef MP4_RECOREDER_H_
#define MP4_RECOREDER_H_

#include "video_codec/video_codec_define.h"

#include <string>

extern "C"
{
#include <libavformat/avformat.h>
}

namespace nvr
{
class MP4Muxer
{
public:
    MP4Muxer();

    ~MP4Muxer();

    int32_t Initialize(const std::string &filename,
                       int32_t width,
                       int32_t height,
                       int32_t frame_rate,
                       VideoCodecType codec_type);

    int32_t WriteVideoFrame(const VideoFrame &frame);

    void Close();

private:
    int32_t WriteH264Frame(const VideoFrame &frame);
    int32_t WriteH265Frame(const VideoFrame &frame);

private:
    VideoCodecType codec_type_;
    std::string vps_;
    std::string sps_;
    std::string pps_;
    std::string sei_;
    AVFormatContext *fmt_ctx_;
    uint64_t frame_index_;
    uint8_t *buf_;
    bool init_;
};

} // namespace nvr
#endif