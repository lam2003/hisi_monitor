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

int32_t VideoCaptureImpl::Initialize()
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    //初始化mipi(Mobile Idustry Processor Interface)
    int32_t fd = open("/dev/hi_mipi", O_RDWR);
    if (fd < 0)
    {
        log_e("open hisi mipi device failed,%s", strerror(errno));
        return static_cast<int>(KDeviceError);
    }

    //开始配置mipi
    ioctl(fd, HI_MIPI_RESET_MIPI, &KMipiCfg.devno);

    //开始配置传感器
    ioctl(fd, HI_MIPI_RESET_SENSOR, &KMipiCfg.devno);

    //配置mipi
    if (ioctl(fd, HI_MIPI_SET_DEV_ATTR, &KMipiCfg))
    {
        log_e("configure mipi failed,%s", strerror(errno));
        return static_cast<int>(KDeviceError);
    }

    usleep(10000); //10ms

    //结束配置mipi
    ioctl(fd, HI_MIPI_UNRESET_MIPI, &KMipiCfg.devno);

    //结束配置传感器
    ioctl(fd, HI_MIPI_UNRESET_SENSOR, &KMipiCfg.devno);

    ::close(fd);

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