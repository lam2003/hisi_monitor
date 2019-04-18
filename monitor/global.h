#pragma once

//stl
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//system
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <errno.h>
//logger
#include <elog.h>
//hisi sdk

#include <hi_common.h>
#include <hi_comm_sys.h>
#include <hi_comm_vb.h>
#include <hi_comm_isp.h>
#include <hi_comm_vi.h>
#include <hi_comm_vo.h>
#include <hi_comm_venc.h>
#include <hi_comm_vpss.h>
#include <hi_comm_region.h>
#include <hi_comm_adec.h>
#include <hi_comm_aenc.h>
#include <hi_comm_ai.h>
#include <hi_comm_ao.h>
#include <hi_comm_aio.h>
#include <hi_defines.h>
#include <hi_mipi.h>
#include <mpi_sys.h>
#include <mpi_vb.h>
#include <mpi_vi.h>
#include <mpi_vo.h>
#include <mpi_venc.h>
#include <mpi_vpss.h>
#include <mpi_region.h>
#include <mpi_adec.h>
#include <mpi_aenc.h>
#include <mpi_ai.h>
#include <mpi_ao.h>
#include <mpi_isp.h>
#include <mpi_ae.h>
#include <mpi_awb.h>
#include <mpi_af.h>
#include <hi_vreg.h>
#include <hi_sns_ctrl.h>

#define NVR_CHECK(condition)                          \
    while (!(condition))                              \
    {                                                 \
        log_e("failed when checking %s", #condition); \
        exit(-1);                                     \
    }

#define PIC_WIDTH 1920                               //mx290图像宽
#define PIC_HEIGHT 1080                              //mx290图像长
#define PIXEL_FORMAT PIXEL_FORMAT_YUV_SEMIPLANAR_420 //使用YUV420P
#define PIC_SIZE PIC_HD1080                          //1080P采样
#define ALIGN 64                                     //默认内存对齐大小
#define VB_POOLS_NUM 128                             //缓冲池数量
#define VB_MEM_BLK_NUM 5                             //内存块数量

#define NVR_ISP_DEV 0  //ISP设备
#define NVR_VI_DEV 0   //VI设备
#define NVR_VI_CHN 0   //VI通道
#define NVR_VPSS_GRP 0 //VPSS组
#define NVR_VPSS_CHN 0 //VPSS通道

#ifdef __cplusplus
extern "C"
{
#endif

    //MX290 mipi初始化参数,海思提供
    static combo_dev_attr_t KMipiCfg =
        {
            .devno = 0,
            .input_mode = INPUT_MODE_LVDS,
            {.lvds_attr =
                 {
                     .img_size = {PIC_WIDTH, PIC_HEIGHT},
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

    static ISP_PUB_ATTR_S KISPPubAttr = {

        .stWndRect = {
            .s32X = 0,
            .s32Y = 0,
            .u32Width = PIC_WIDTH,
            .u32Height = PIC_HEIGHT,
        },
        .f32FrameRate = 30,
        .enBayer = BAYER_GBRG};

    static VI_DEV_ATTR_S KVIDevAttr = {
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
        .stDevRect = {.s32X = 0, .s32Y = 0, .u32Width = PIC_WIDTH, .u32Height = PIC_HEIGHT}};

    static VI_CHN_ATTR_S KVIChnAttr = {
        .stCapRect = {.s32X = 0, .s32Y = 0, .u32Width = PIC_WIDTH, .u32Height = PIC_HEIGHT},
        .stDestSize = {.u32Width = PIC_WIDTH, .u32Height = PIC_HEIGHT},
        .enCapSel = VI_CAPSEL_BOTH, //逐行采样
        .enPixFormat = PIXEL_FORMAT,
        .enCompressMode = COMPRESS_MODE_NONE,
        .bMirror = HI_FALSE,
        .bFlip = HI_FALSE,
        .s32SrcFrameRate = -1, //自适应
        .s32DstFrameRate = -1};

#ifdef __cplusplus
}
#endif

// arm-hisiv500-linux-g++不支持
// template<typename T>
// T GLOBAL;