
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

struct H264Frame : public VideoFrame
{
    enum NaluType
    {
        PSLICE = 1,
        ISLICE = 5,
        SEI = 6,
        SPS = 7,
        PPS = 8,
    };

    ~H264Frame() override {}

    int32_t GetCodecType() const override
    {
        return static_cast<int>(H264);
    }
};

struct H265Frame : public VideoFrame
{
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

    ~H265Frame() override {}

    int32_t GetCodecType() const override
    {
        return static_cast<int>(H265);
    }
};

}; // namespace nvr
#endif