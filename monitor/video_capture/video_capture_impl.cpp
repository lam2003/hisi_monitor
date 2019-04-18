#include "video_capture/video_capture_impl.h"
#include "base/ref_counted_object.h"
#include "common/res_code.h"

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
    ioctl(fd, HI_MIPI_RESET_MIPI, &KMipiCfg.devno);

    //开始配置传感器
    ioctl(fd, HI_MIPI_RESET_SENSOR, &KMipiCfg.devno);

    //配置mipi
    if (ioctl(fd, HI_MIPI_SET_DEV_ATTR, &KMipiCfg))
    {
        log_e("configure mipi failed,%s", strerror(errno));
        return static_cast<int>(KMPPError);
    }

    usleep(10000); //10ms

    //结束配置mipi
    ioctl(fd, HI_MIPI_UNRESET_MIPI, &KMipiCfg.devno);

    //结束配置传感器
    ioctl(fd, HI_MIPI_UNRESET_SENSOR, &KMipiCfg.devno);

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

    ret = HI_MPI_ISP_SetPubAttr(NVR_ISP_DEV, &KISPPubAttr);
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

void VideoCaptureImpl::StartISP()
{
    isp_thread_ = std::unique_ptr<std::thread>(new std::thread([]() {
        //设置线程名
        prctl(PR_SET_NAME, "hisi_isp_thread", 0, 0, 0);

        HI_MPI_ISP_Run(NVR_ISP_DEV);
    }));

    usleep(1000); //10ms
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

    ret = HI_MPI_VI_SetDevAttr(NVR_VI_DEV, &KVIDevAttr);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VI_SetDevAttr failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    VI_WDR_ATTR_S wdr_attr;
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

int32_t VideoCaptureImpl::StartVIChn()
{
    int32_t ret;

    ret = HI_MPI_VI_SetChnAttr(NVR_VI_CHN, &KVIChnAttr);
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



void VideoCaptureImpl::close()
{
}

VideoCaptureImpl::VideoCaptureImpl() : vi_chn_fd_(-1),
                                       isp_thread_(nullptr),
                                       init_(false)
{
}

VideoCaptureImpl::~VideoCaptureImpl()
{
    close();
}
}; // namespace nvr