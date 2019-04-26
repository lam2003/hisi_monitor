#pragma once

//stl
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
//system
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
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

#define CHACK_ERROR(a)                                           \
    if (KSuccess != a)                                           \
    {                                                            \
        log_e("error:%s", make_error_code(a).message().c_str()); \
        return static_cast<int>(a);                              \
    }

#define PIC_WIDTH 1920                               //mx290图像宽
#define PIC_HEIGHT 1080                              //mx290图像长
#define FRAME_RATE 30                                //mx290源帧率
#define PIXEL_FORMAT PIXEL_FORMAT_YUV_SEMIPLANAR_420 //使用YUV420P
#define PIC_SIZE PIC_HD1080                          //1080P采样
#define ALIGN 64                                     //默认内存对齐大小
#define VB_POOLS_NUM 128                             //缓冲池数量
#define VB_MEM_BLK_NUM 5                             //内存块数量
#define BUFFER_SIZE 262144                           //缓存大小

#define NVR_ISP_DEV 0  //ISP设备
#define NVR_VI_DEV 0   //VI设备
#define NVR_VI_CHN 0   //VI通道
#define NVR_VPSS_GRP 0 //VPSS组
#define NVR_VPSS_CHN 1 //VPSS通道
#define NVR_VENC_CHN 0 //VENC通道

// arm-hisiv500-linux-g++不支持
// template<typename T>
// T GLOBAL;