#ifndef VIDEO_CODEC_DEFINE_H_
#define VIDEO_CODEC_DEFINE_H_

#include "video/video_frame.h"

#include <string>

namespace nvr
{

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
};

static VideoCodecMode StringToCodecMode(const std::string &str){

    if (strcasecmp(str.c_str(), "CBR") == 0)
        return CBR;

    if (strcasecmp(str.c_str(), "VBR") == 0)
        return VBR;

    if (strcasecmp(str.c_str(), "AVBR") == 0)
        return AVBR;

    return CBR;
}

}; // namespace nvr
#endif