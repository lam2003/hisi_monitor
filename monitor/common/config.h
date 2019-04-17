#ifndef CONFIG_H_
#define CONFIG_H_

#include <mutex>
#include <string>

#define PIXEL_FORMAT PIXEL_FORMAT_YUV_SEMIPLANAR_420 //使用YUV420P
#define PIC_SIZE PIC_HD1080                          //与相机有关,现使用SONY MX290,分辨率1080P
#define ALIGN 64                                     //默认内存对齐大小
#define VB_POOLS_NUM 128                             //缓冲池数量

#define NVR_ISP_DEV 0

#define NVR_VI_DEV 0

#define NVR_VI_CHN 0

#ifdef __cplusplus
extern "C"
{
#endif
  extern combo_dev_attr_t KMipiCfg;
  extern ISP_PUB_ATTR_S KISPPubAttr;
  extern VI_DEV_ATTR_S KVIDevAttr;
  extern VI_CHN_ATTR_S KVIChnAttr;
#ifdef __cplusplus
}
#endif

namespace nvr
{
struct Config
{
    struct System
    {
        int vb_pools_num;
        System() : vb_pools_num(VB_POOLS_NUM)
        {
        }
    };
    struct Video
    {
        PIC_SIZE_E pic_size;

        Video() : pic_size(PIC_SIZE)
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