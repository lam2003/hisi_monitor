
#ifndef VIDEO_CODEC_DEFINE_H_
#define VIDEO_CODEC_DEFINE_H_

#include "video/video_frame.h"

namespace nvr
{
enum VideoCodecType
{
    H264 = 0,
    H265
};

enum VideoCodecMode
{
    CBR = 0,
    VBR,
    AVBR
};

class H265Frame : public VideoFrame
{
  public:
    enum NaluType
    {
        BSLICE = 0,
        PSLICE = 1,
        ISLICE = 2,
        IDRSLICE = 19,
        VPS = 32,
        SPS = 33,
        PPS = 34,
        SEI = 39
    };
    
    NaluType type;
    uint8_t *data;
    uint32_t len;
    uint64_t ts;

    ~H265Frame() override {}
};

}; // namespace nvr
#endif