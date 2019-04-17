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

void VideoCaptureImpl::RegisterCaptureDataCallback(VideoSinkInterface<VIDEO_FRAME_INFO_S> *video_sink)
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

int32_t VideoCaptureImpl::InitISP()
{
    int32_t ret;

    ret = sensor_register_callback();
    if (HI_SUCCESS != ret)
    {
        log_e("sensor_register_callback failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    ALG_LIB_S alg_lib;
    memset(&alg_lib, 0, sizeof(alg_lib));

    alg_lib.s32Id = 0;
    strncpy(alg_lib.acLibName, HI_AE_LIB_NAME, 20);
    ret = HI_MPI_AE_Register(NVR_ISP_DEV, &alg_lib);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_AE_Register failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    alg_lib.s32Id = 0;
    strncpy(alg_lib.acLibName, HI_AWB_LIB_NAME, 20);
    ret = HI_MPI_AWB_Register(NVR_ISP_DEV, &alg_lib);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_AWB_Register failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    alg_lib.s32Id = 0;
    strncpy(alg_lib.acLibName, HI_AF_LIB_NAME, 20);
    ret = HI_MPI_AF_Register(NVR_ISP_DEV, &alg_lib);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_AF_Register failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    ret = HI_MPI_ISP_MemInit(NVR_ISP_DEV);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_Init failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    ISP_WDR_MODE_S wdr_mode;
    memset(&wdr_mode, 0, sizeof(wdr_mode));
    wdr_mode.enWDRMode = WDR_MODE_NONE;
    ret = HI_MPI_ISP_SetWDRMode(NVR_ISP_DEV, &wdr_mode);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_SetWDRMode failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    ret = HI_MPI_ISP_SetPubAttr(NVR_ISP_DEV, &KISPPubAttr);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_SetPubAttr failed,code %#x", ret);
        return static_cast<int>(KISPError);
    }

    ret = HI_MPI_ISP_Init(NVR_ISP_DEV);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_ISP_Init failed,code %#x", ret);
        return static_cast<int>(KISPError);
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
        return static_cast<int>(KVIError);
    }

    ret = HI_MPI_VI_SetDevAttr(NVR_VI_DEV, &KVIDevAttr);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VI_SetDevAttr failed,code %#x", ret);
        return static_cast<int>(KVIError);
    }

    VI_WDR_ATTR_S wdr_attr;
    wdr_attr.enWDRMode = wdr_mode.enWDRMode;
    wdr_attr.bCompress = HI_FALSE;

    ret = HI_MPI_VI_SetWDRAttr(NVR_VI_DEV, &wdr_attr);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VI_SetWDRAttr failed,code %#x", ret);
        return static_cast<int>(KVIError);
    }

    ret = HI_MPI_VI_EnableDev(NVR_VI_DEV);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VI_EnableDev failed,code %#x", ret);
        return static_cast<int>(KVIError);
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
        return static_cast<int>(KVIChnError);
    }

    ret = HI_MPI_VI_EnableChn(NVR_VI_CHN);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VI_EnableChn failed,code %#x", ret);
        return static_cast<int>(KVIChnError);
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

int32_t VideoCaptureImpl::StartCapture()
{
    if (!init_)
        return static_cast<int>(KUnInitialize);

    //设置VI缓存帧数,默认为0
    int32_t ret;

    ret = HI_MPI_VI_SetFrameDepth(NVR_VI_CHN, 3);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VI_SetFrameDepth failed,code %#x", ret);
        return static_cast<int>(KVIChnError);
    }

    vi_chn_fd_ = HI_MPI_VI_GetFd(NVR_VI_CHN);
    if (vi_chn_fd_ < 0)
    {
        log_e("HI_MPI_VI_GetFd failed");
        return static_cast<int>(KVIChnError);
    }

    run_ = true;
    capture_thread_ = std::unique_ptr<std::thread>(new std::thread([this]() {
        int32_t ret;
        struct timeval tv;
        fd_set fds;
        VIDEO_FRAME_INFO_S frame_info;

        int i = 0;
        while (run_)
        {
            tv.tv_sec = 1; //1 second
            tv.tv_usec = 0;

            FD_ZERO(&fds);
            FD_SET(vi_chn_fd_, &fds);

            ret = select(vi_chn_fd_ + 1, &fds, NULL, NULL, &tv);
            if (ret < 0)
            {
                log_e("select error,%s", strerror(errno));
                return;
            }

            if (ret != 0 && FD_ISSET(vi_chn_fd_, &fds))
            {
                ret = HI_MPI_VI_GetFrame(NVR_VI_CHN, &frame_info, -1);
                if (HI_SUCCESS != ret)
                {
                    log_e("HI_MPI_VI_GetFrame failed,code %#x", ret);
                    return;
                }

                int32_t u_len = frame_info.stVFrame.u32Width * frame_info.stVFrame.u32Height;
                
                frame_info.stVFrame.pVirAddr[0] =
                    HI_MPI_SYS_Mmap(frame_info.stVFrame.u32PhyAddr[0], u_len * 1.5);

                HI_MPI_SYS_Munmap(frame_info.stVFrame.pVirAddr[0], u_len * 1.5);

                log_d("got a video frame ");
                ret = HI_MPI_VI_ReleaseFrame(NVR_VI_CHN, &frame_info);
                if (HI_SUCCESS != ret)
                {
                    log_e("HI_MPI_VI_ReleaseFrame failed,code %#x", ret);
                    return;
                }
            }
        }
    }));

    return static_cast<int>(KSuccess);
}

int32_t VideoCaptureImpl::StopCapture()
{
    if (!init_)
        return static_cast<int>(KUnInitialize);

    if (run_)
    {
    }

    return static_cast<int>(KSuccess);
}

void VideoCaptureImpl::close()
{
}

VideoCaptureImpl::VideoCaptureImpl() : capture_thread_(nullptr),
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