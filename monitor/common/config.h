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
            frame_rate = 25;
            width = 1280;
            height = 720;

            codec_type = H265;
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

    System system;
    Video video;

    static Config *Instance()
    {
        static Config *instance = new Config;
        return instance;
    }
};
} // namespace nvr

#endif