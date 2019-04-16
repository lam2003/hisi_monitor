#include "video_capture/video_capture_impl.h"
#include "base/ref_counted_object.h"
#include "common/res_code.h"
#include "common/config.h"
#include "common/utils.h"

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

void VideoCaptureImpl::RegisterCaptureDataCallback(VideoSinkInterface<VideoFrame> *video_sink)
{
    video_sink_ = video_sink;
}

void VideoCaptureImpl::DeRegisterCaptureDataCallback()
{
    video_sink_ = nullptr;
}

int32_t VideoCaptureImpl::StartMIPI()
{
    //初始化mipi(Mobile Idustry Processor Interface)
    int32_t fd = open("/dev/hi_mipi", O_RDWR);
    if (fd < 0)
    {
        log_e("open hisi mipi device failed,%s", strerror(errno));
        return static_cast<int>(KMIPIError);
    }

    //开始配置mipi
    ioctl(fd, HI_MIPI_RESET_MIPI, &KMipiCfg.devno);

    //开始配置传感器
    ioctl(fd, HI_MIPI_RESET_SENSOR, &KMipiCfg.devno);

    //配置mipi
    if (ioctl(fd, HI_MIPI_SET_DEV_ATTR, &KMipiCfg))
    {
        log_e("configure mipi failed,%s", strerror(errno));
        return static_cast<int>(KMIPIError);
    }

    usleep(10000); //10ms

    //结束配置mipi
    ioctl(fd, HI_MIPI_UNRESET_MIPI, &KMipiCfg.devno);

    //结束配置传感器
    ioctl(fd, HI_MIPI_UNRESET_SENSOR, &KMipiCfg.devno);

    ::close(fd);

    return static_cast<int>(KSuccess);
}

int32_t VideoCaptureImpl::StartISP()
{
    int32_t ret;

    ret = sensor_register_callback();
    if (HI_SUCCESS != ret)
    {
        log_e("sensor_register_callback failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    ISP_DEV isp_dev;
    memset(&isp_dev, 0, sizeof(isp_dev));

    ALG_LIB_S alg_lib;
    memset(&alg_lib, 0, sizeof(alg_lib));

    alg_lib.s32Id = 0;
    strncpy(alg_lib.acLibName, HI_AE_LIB_NAME, 20);
    ret = HI_MPI_AE_Register(isp_dev, &alg_lib);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_AE_Register failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    alg_lib.s32Id = 0;
    strncpy(alg_lib.acLibName, HI_AWB_LIB_NAME, 20);
    ret = HI_MPI_AWB_Register(isp_dev, &alg_lib);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_AWB_Register failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    alg_lib.s32Id = 0;
    strncpy(alg_lib.acLibName, HI_AF_LIB_NAME, 20);
    ret = HI_MPI_AF_Register(isp_dev, &alg_lib);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_AF_Register failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    ret = HI_MPI_ISP_MemInit(isp_dev);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_Init failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    ISP_WDR_MODE_S wdr_mode;
    memset(&wdr_mode, 0, sizeof(wdr_mode));
    wdr_mode.enWDRMode = WDR_MODE_NONE;
    ret = HI_MPI_ISP_SetWDRMode(0, &wdr_mode);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_SetWDRMode failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    ret = HI_MPI_ISP_SetPubAttr(isp_dev, &KISPPubAttr);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_SetPubAttr failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    ret = HI_MPI_ISP_Init(isp_dev);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_Init failed,code %#x", ret);
        return static_cast<int>(KISPError);
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

    code = static_cast<err_code>(StartISP());
    if (KSuccess != code)
        return code;
    return static_cast<int>(KSuccess);
}

int32_t VideoCaptureImpl::StartCapture()
{
    return 0;
}

int32_t VideoCaptureImpl::StopCapture()
{
    return 0;
}

bool VideoCaptureImpl::CaptureStarted()
{
    return 0;
}

void VideoCaptureImpl::close()
{
}

VideoCaptureImpl::VideoCaptureImpl() : thread_(nullptr),
                                       run_(false),
                                       video_sink_(nullptr),
                                       init_(false)
{
}

VideoCaptureImpl::~VideoCaptureImpl()
{
    close();
}

}; // namespace nvr