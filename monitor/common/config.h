#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include "video_codec/video_codec_define.h"

namespace nvr
{
struct Config
{
    struct System
    {
        System()
        {
            font_path = "/nfsroot/msyh.ttc";
            font_size = 32;
        }

        std::string font_path;
        int32_t font_size;
    };
    struct Video
    {
        Video()
        {
            frame_rate = 20;
            width = 1280;
            height = 720;
            codec_type = H264;
            codec_mode = CBR;
            codec_profile = 0;
            codec_bitrate = 2048; //k
        }
        //common
        int32_t frame_rate;
        int32_t width;
        int32_t height;

        //codec configure
        VideoCodecType codec_type;
        VideoCodecMode codec_mode;
        int32_t codec_profile;
        int32_t codec_bitrate;
    };

    struct Detect
    {
        Detect()
        {
            width = 720;
            height = 480;
        }

        int32_t width;
        int32_t height;
    };

    struct Rtmp
    {
        Rtmp()
        {
            url = "rtmp://192.168.22.222/live/test";
        }

        std::string url;
    };

    System system;
    Video video;
    Detect detect;

    static Config *Instance()
    {
        static Config *instance = new Config;
        return instance;
    }
};
} // namespace nvr

#endif