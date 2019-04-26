#include "video_capture/video_capture_impl.h"
#include "base/ref_counted_object.h"
#include "common/res_code.h"

static combo_dev_attr_t COMBO_DEV_ATTR =
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

static ISP_PUB_ATTR_S ISP_PUB_ATTR = {

    .stWndRect = {
        .s32X = 0,
        .s32Y = 0,
        .u32Width = PIC_WIDTH,
        .u32Height = PIC_HEIGHT,
    },
    .f32FrameRate = FRAME_RATE,
    .enBayer = BAYER_GBRG};

static VI_DEV_ATTR_S VI_DEV_ATTR = {

    .enIntfMode = VI_MODE_LVDS,

    .enWorkMode = VI_WORK_MODE_1Multiplex,

    .au32CompMask = {0xFFF00000, 0x0},

    .enScanMode = VI_SCAN_PROGRESSIVE,

    .s32AdChnId = {-1, -1, -1, -1},

    .enDataSeq = VI_INPUT_DATA_YUYV,

    .stSynCfg = {
        .enVsync = VI_VSYNC_PULSE,
        .enVsyncNeg = VI_VSYNC_NEG_LOW,
        .enHsync = VI_HSYNC_VALID_SINGNAL,
        .enHsyncNeg = VI_HSYNC_NEG_HIGH,
        .enVsyncValid = VI_VSYNC_VALID_SINGAL,
        .enVsyncValidNeg = VI_VSYNC_VALID_NEG_HIGH,

        .stTimingBlank = {
            .u32HsyncHfb = 0,
            .u32HsyncAct = 1280,
            .u32HsyncHbb = 0,

            .u32VsyncVfb = 0,
            .u32VsyncVact = 720,
            .u32VsyncVbb = 0,

            .u32VsyncVbfb = 0,
            .u32VsyncVbact = 0,
            .u32VsyncVbbb = 0}},

    .enDataPath = VI_PATH_ISP,

    .enInputDataType = VI_DATA_TYPE_RGB,

    .bDataRev = HI_FALSE,

    .stDevRect = {.s32X = 0, .s32Y = 0, .u32Width = PIC_WIDTH, .u32Height = PIC_HEIGHT}};

static VI_CHN_ATTR_S VI_CHN_ATTR = {
    .stCapRect = {.s32X = 0, .s32Y = 0, .u32Width = PIC_WIDTH, .u32Height = PIC_HEIGHT},
    .stDestSize = {.u32Width = PIC_WIDTH, .u32Height = PIC_HEIGHT},
    .enCapSel = VI_CAPSEL_BOTH,
    .enPixFormat = PIXEL_FORMAT,
    .enCompressMode = COMPRESS_MODE_NONE,
    .bMirror = HI_FALSE,
    .bFlip = HI_FALSE,
    .s32SrcFrameRate = FRAME_RATE,
    .s32DstFrameRate = FRAME_RATE};

namespace nvr
{
rtc::scoped_refptr<VideoCaptureModule> VideoCaptureImpl::Create()
{
    err_code code;

    rtc::scoped_refptr<VideoCaptureImpl> implemention(new rtc::RefCountedObject<VideoCaptureImpl>());

    code = static_cast<err_code>(implemention->Initialize());

    if (KSuccess != code)
    {
        log_e("error:%s", make_error_code(code).message().c_str());
        return nullptr;
    }

    return implemention;
}

int32_t VideoCaptureImpl::StartMIPI()
{
    //初始化mipi(Mobile Idustry Processor Interface)
    int32_t fd = open("/dev/hi_mipi", O_RDWR);
    if (fd < 0)
    {
        log_e("open hisi mipi device failed,%s", strerror(errno));
        return static_cast<int>(KMPPError);
    }

    //开始配置mipi
    ioctl(fd, HI_MIPI_RESET_MIPI, &COMBO_DEV_ATTR.devno);

    //开始配置传感器
    ioctl(fd, HI_MIPI_RESET_SENSOR, &COMBO_DEV_ATTR.devno);

    //配置mipi
    if (ioctl(fd, HI_MIPI_SET_DEV_ATTR, &COMBO_DEV_ATTR))
    {
        log_e("configure mipi failed,%s", strerror(errno));
        return static_cast<int>(KMPPError);
    }

    usleep(10000); //10ms

    //结束配置mipi
    ioctl(fd, HI_MIPI_UNRESET_MIPI, &COMBO_DEV_ATTR.devno);

    //结束配置传感器
    ioctl(fd, HI_MIPI_UNRESET_SENSOR, &COMBO_DEV_ATTR.devno);

    ::close(fd);

    return static_cast<int>(KSuccess);
}

int32_t VideoCaptureImpl::InitISP()
{
    int32_t ret;

    ret = sensor_register_callback();
    if (HI_SUCCESS != ret)
    {
        log_e("sensor_register_callback failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ALG_LIB_S alg_lib;
    memset(&alg_lib, 0, sizeof(alg_lib));

    alg_lib.s32Id = 0;
    strncpy(alg_lib.acLibName, HI_AE_LIB_NAME, 20);
    ret = HI_MPI_AE_Register(NVR_ISP_DEV, &alg_lib);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_AE_Register failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    alg_lib.s32Id = 0;
    strncpy(alg_lib.acLibName, HI_AWB_LIB_NAME, 20);
    ret = HI_MPI_AWB_Register(NVR_ISP_DEV, &alg_lib);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_AWB_Register failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    alg_lib.s32Id = 0;
    strncpy(alg_lib.acLibName, HI_AF_LIB_NAME, 20);
    ret = HI_MPI_AF_Register(NVR_ISP_DEV, &alg_lib);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_AF_Register failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_ISP_MemInit(NVR_ISP_DEV);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_Init failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ISP_WDR_MODE_S wdr_mode;
    memset(&wdr_mode, 0, sizeof(wdr_mode));
    wdr_mode.enWDRMode = WDR_MODE_NONE;
    ret = HI_MPI_ISP_SetWDRMode(NVR_ISP_DEV, &wdr_mode);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_SetWDRMode failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_ISP_SetPubAttr(NVR_ISP_DEV, &ISP_PUB_ATTR);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_SetPubAttr failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_ISP_Init(NVR_ISP_DEV);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_Init failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
};

void VideoCaptureImpl::UnInitISP()
{
    int32_t ret;

    ALG_LIB_S alg_lib;
    memset(&alg_lib, 0, sizeof(alg_lib));

    alg_lib.s32Id = 0;
    strncpy(alg_lib.acLibName, HI_AF_LIB_NAME, 20);
    ret = HI_MPI_AF_UnRegister(NVR_ISP_DEV, &alg_lib);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_AF_UnRegister failed,code %#x", ret);

    alg_lib.s32Id = 0;
    strncpy(alg_lib.acLibName, HI_AWB_LIB_NAME, 20);
    ret = HI_MPI_AWB_UnRegister(NVR_ISP_DEV, &alg_lib);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_AWB_UnRegister failed,code %#x", ret);

    alg_lib.s32Id = 0;
    strncpy(alg_lib.acLibName, HI_AE_LIB_NAME, 20);
    ret = HI_MPI_AE_UnRegister(NVR_ISP_DEV, &alg_lib);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_AE_UnRegister failed,code %#x", ret);

    ret = sensor_unregister_callback();
    if (HI_SUCCESS != ret)
        log_e("sensor_unregister_callback failed,code %#x", ret);
}

void VideoCaptureImpl::StartISP()
{
    thread_ = std::unique_ptr<std::thread>(new std::thread([this]() {
        int32_t ret;
        ret = HI_MPI_ISP_Run(NVR_ISP_DEV);
        if (HI_SUCCESS != ret)
            log_e("HI_MPI_ISP_Run failed,code %#x", ret);
    }));

    usleep(10000); //10ms
}

void VideoCaptureImpl::StopISP()
{
    int32_t ret;

    ret = HI_MPI_ISP_Exit(NVR_ISP_DEV);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_ISP_Exit failed,code %#x", ret);

    thread_->join();
    thread_.reset();
    thread_ = nullptr;
}

int32_t VideoCaptureImpl::StartVI()
{
    int32_t ret;

    ISP_WDR_MODE_S wdr_mode;
    wdr_mode.enWDRMode = WDR_MODE_NONE;

    ret = HI_MPI_ISP_GetWDRMode(NVR_ISP_DEV, &wdr_mode);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_GetWDRMode failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_VI_SetDevAttr(NVR_VI_DEV, &VI_DEV_ATTR);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VI_SetDevAttr failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    VI_WDR_ATTR_S wdr_attr;
    memset(&wdr_attr, 0, sizeof(wdr_attr));
    wdr_attr.enWDRMode = wdr_mode.enWDRMode;
    wdr_attr.bCompress = HI_FALSE;

    ret = HI_MPI_VI_SetWDRAttr(NVR_VI_DEV, &wdr_attr);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VI_SetWDRAttr failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_VI_EnableDev(NVR_VI_DEV);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VI_EnableDev failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
};

void VideoCaptureImpl::StopVI()
{
    int32_t ret;

    ret = HI_MPI_VI_DisableDev(NVR_VI_DEV);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_VI_DisableDev failed,code %#x", ret);
};

int32_t VideoCaptureImpl::StartVIChn()
{
    int32_t ret;

    ret = HI_MPI_VI_SetChnAttr(NVR_VI_CHN, &VI_CHN_ATTR);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VI_SetChnAttr failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_VI_EnableChn(NVR_VI_CHN);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VI_EnableChn failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
};

void VideoCaptureImpl::StopVIChn()
{
    int32_t ret;

    ret = HI_MPI_VI_DisableChn(NVR_VI_CHN);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_VI_DisableChn failed,code %#x", ret);
}

int32_t VideoCaptureImpl::Initialize()
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    err_code code;

    code = static_cast<err_code>(StartMIPI());
    if (KSuccess != code)
        return code;

    code = static_cast<err_code>(InitISP());
    if (KSuccess != code)
        return code;

    StartISP();

    code = static_cast<err_code>(StartVI());
    if (KSuccess != code)
        return code;

    code = static_cast<err_code>(StartVIChn());
    if (KSuccess != code)
        return code;

    init_ = true;

    return static_cast<int>(KSuccess);
}

void VideoCaptureImpl::Close()
{
    if (!init_)
        return;

    StopVIChn();

    StopVI();

    StopISP();

    UnInitISP();

    init_ = false;
}

VideoCaptureImpl::VideoCaptureImpl() : thread_(nullptr),
                                       init_(false)
{
}

VideoCaptureImpl::~VideoCaptureImpl()
{
    Close();
}
}; // namespace nvr