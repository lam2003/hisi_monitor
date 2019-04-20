#ifndef CONFIG_H_
#define CONFIG_H_

#include <mutex>
#include <string>


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