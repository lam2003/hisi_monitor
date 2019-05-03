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
            codec_type = H264;
            codec_mode = CBR;
            codec_profile = 2;
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
            trigger_thresh = 1;
        }
        int32_t trigger_thresh;
    };
    struct Record
    {
        Record()
        {
            segment_duration = 3600; //60mins
            path = "/nfsroot/record/fuck1";
            use_md = true;
            md_duration = 10; //5mins
        };

        int32_t segment_duration;
        std::string path;
        bool use_md;
        int32_t md_duration;
    };

    struct Rtmp
    {
        Rtmp()
        {
            // url = "rtmp://192.168.22.222:1935/live/test";
            url = "rtmp://127.0.0.1:1935/live/test";
        }

        std::string url;
    };

    System system;
    Video video;
    Detect detect;
    Rtmp rtmp;
    Record record;

    static Config *Instance()
    {
        static Config *instance = new Config;
        return instance;
    }
};
} // namespace nvr

#endif