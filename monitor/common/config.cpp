#include "common/config.h"

#ifdef __cplusplus
extern "C"
{
#endif

    //MX290 mipi初始化参数,海思提供
    combo_dev_attr_t KMipiCfg =
        {
            .devno = 0,
            .input_mode = INPUT_MODE_LVDS,
            {.lvds_attr =
                 {
                     .img_size = {1920, 1080},
                     .raw_data_type = RAW_DATA_12BIT,
                     .wdr_mode = HI_WDR_MODE_NONE,

                     .sync_mode = LVDS_SYNC_MODE_SAV,
                     .vsync_type = {LVDS_VSYNC_NORMAL, 0, 0},
                     .fid_type = {LVDS_FID_NONE, HI_FALSE},

                     .data_endian = LVDS_ENDIAN_BIG,
                     .sync_code_endian = LVDS_ENDIAN_BIG,
                     .lane_id = {0, 1, 2, 3},

                     .sync_code =
                         {
                             {{0xab0, 0xb60, 0x800, 0x9d0},
                              {0xab0, 0xb60, 0x800, 0x9d0},
                              {0xab0, 0xb60, 0x800, 0x9d0},
                              {0xab0, 0xb60, 0x800, 0x9d0}},
                             {{0xab0, 0xb60, 0x800, 0x9d0},
                              {0xab0, 0xb60, 0x800, 0x9d0},
                              {0xab0, 0xb60, 0x800, 0x9d0},
                              {0xab0, 0xb60, 0x800, 0x9d0}},
                             {{0xab0, 0xb60, 0x800, 0x9d0},
                              {0xab0, 0xb60, 0x800, 0x9d0},
                              {0xab0, 0xb60, 0x800, 0x9d0},
                              {0xab0, 0xb60, 0x800, 0x9d0}},
                             {{0xab0, 0xb60, 0x800, 0x9d0},
                              {0xab0, 0xb60, 0x800, 0x9d0},
                              {0xab0, 0xb60, 0x800, 0x9d0},
                              {0xab0, 0xb60, 0x800, 0x9d0}}}}}};

    ISP_PUB_ATTR_S KISPPubAttr = {

        .stWndRect = {
            .s32X = 0,
            .s32Y = 0,
            .u32Width = 1920,
            .u32Height = 1080,
        },
        .f32FrameRate = 30,
        .enBayer = BAYER_GBRG};

#ifdef __cplusplus
}
#endif