#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include "video_codec/video_codec_define.h"

namespace nvr
{
struct Config
{
    struct Video
    {
        Video()
        {
            frame_rate = 25;
            width = 1280;
            height = 720;
            codec_mode = CBR;
            codec_profile = 0;
            codec_bitrate = 2000; 
        }
  
        int32_t frame_rate;
        int32_t width;
        int32_t height;
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
            segment_duration = 3600; //second
            path = "/app/record";
            use_md = true;
            md_duration = 60; //second
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
            url = "rtmp://127.0.0.1:1935/live/monitor";
        }

        std::string url;
    };

    Video video;
    Detect detect;
    Rtmp rtmp;
    Record record;

    static Config *Instance()
    {
        static Config *instance = new Config;
        return instance;
    }

    int32_t ReadConfigFile(const std::string &config_file);
    
    
};
} // namespace nvr

#endif