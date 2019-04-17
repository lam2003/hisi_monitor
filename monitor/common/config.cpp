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

    VI_DEV_ATTR_S KVIDevAttr = {
        /* interface mode */
        .enIntfMode = VI_MODE_LVDS,
        /* multiplex mode */
        .enWorkMode = VI_WORK_MODE_1Multiplex,
        /* r_mask    g_mask    b_mask*/
        .au32CompMask = {0xFFF00000, 0x0},
        /* progessive or interleaving */
        .enScanMode = VI_SCAN_PROGRESSIVE,
        /*AdChnId*/
        .s32AdChnId = {-1, -1, -1, -1},
        /*enDataSeq, only support yuv*/
        .enDataSeq = VI_INPUT_DATA_YUYV,

        /* synchronization information */
        .stSynCfg = {
            /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
            .enVsync = VI_VSYNC_PULSE,
            .enVsyncNeg = VI_VSYNC_NEG_LOW,
            .enHsync = VI_HSYNC_VALID_SINGNAL,
            .enHsyncNeg = VI_HSYNC_NEG_HIGH,
            .enVsyncValid = VI_VSYNC_VALID_SINGAL,
            .enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,

            /*hsync_hfb    hsync_act    hsync_hhb*/
            .stTimingBlank = {
                .u32HsyncHfb = 0,
                .u32HsyncAct = 1280,
                .u32HsyncHbb = 0,
                /*vsync0_vhb vsync0_act vsync0_hhb*/
                .u32VsyncVfb = 0,
                .u32VsyncVact = 720,
                .u32VsyncVbb = 0,
                /*vsync1_vhb vsync1_act vsync1_hhb*/
                .u32VsyncVbfb = 0,
                .u32VsyncVbact = 0,
                .u32VsyncVbbb = 0}},
        /* use interior ISP */
        .enDataPath = VI_PATH_ISP,
        /* input data type */
        .enInputDataType = VI_DATA_TYPE_RGB,
        /* bRever */
        .bDataRev = HI_FALSE,
        /* DEV CROP */
        .stDevRect = {.s32X = 0, .s32Y = 0, .u32Width = 1920, .u32Height = 1080}};

    VI_CHN_ATTR_S KVIChnAttr = {
        .stCapRect = {.s32X = 0, .s32Y = 0, .u32Width = 1920, .u32Height = 1080},
        .stDestSize = {.u32Width = 1920, .u32Height = 1080},
        .enCapSel = VI_CAPSEL_BOTH, //逐行采样
        .enPixFormat = PIXEL_FORMAT,
        .enCompressMode = COMPRESS_MODE_NONE,
        .bMirror = HI_FALSE,
        .bFlip = HI_FALSE,
        .s32SrcFrameRate = -1, //自适应
        .s32DstFrameRate = -1
    };

#ifdef __cplusplus
}
#endif