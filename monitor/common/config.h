#ifndef CONFIG_H_
#define CONFIG_H_

#include <mutex>
#include <string>

#define PIXEL_FORMAT PIXEL_FORMAT_YUV_SEMIPLANAR_420 //1.5 bytes per pixel
#define PIC_SIZE PIC_HD1080                          //sony mx290 with 1080p resolution
#define ALIGN 64                                     //default align

namespace nvr
{
struct Config
{
    struct System
    {
        int vb_pools_num;
        System() : vb_pools_num(128)
        {
        }
    };
    struct Video
    {
        PIC_SIZE_E pic_size;

        Video() : pic_size(PIC_HD1080)
        {
        }
    };

    System system;
    Video video;
    std::mutex mux;

    static Config *Instance()
    {
        static Config *instance = new Config;
        return instance;
    }
};

} // namespace nvr

#endif