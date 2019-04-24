#include "common/system.h"
#include "common/res_code.h"
#include "common/config.h"
#include "video_capture/video_capture_impl.h"
#include "video_process/video_process_impl.h"
#include "video_codec/video_codec_impl.h"

#include "video_process/osd.h"

using namespace nvr;

#define CHACK_ERROR(a)                                           \
    if (KSuccess != a)                                           \
    {                                                            \
        log_e("error:%s", make_error_code(a).message().c_str()); \
        return static_cast<int>(a);                              \
    }

int main(int argc, char **argv)
{
    err_code code;
    uint64_t start_time, end_time;

    //初始化日志系统
    System::InitLogger();

    //初始化海思sdk
    log_i("initializing mpp...");

    code = static_cast<err_code>(System::InitMPP());
    CHACK_ERROR(code)

    //初始化视频采集模块
    log_i("initializing video capture...");

    rtc::scoped_refptr<VideoCaptureModule> video_capture_module = VideoCaptureImpl::Create();
    NVR_CHECK(NULL != video_capture_module)

    //初始化视频处理模块
    log_i("initializing video process...");

    rtc::scoped_refptr<VideoProcessModule> video_process_module = VideoProcessImpl::Create({Config::Instance()->video.frame_rate,
                                                                                            Config::Instance()->video.width,
                                                                                            Config::Instance()->video.height});
    NVR_CHECK(NULL != video_process_module)

    log_i("binding video capture and video process...");
    code = static_cast<err_code>(System::VIBindVPSS());
    CHACK_ERROR(code)

    log_i("initializeing video encode...");
    rtc::scoped_refptr<VideoCodecModule> video_codec_module = VideoCodecImpl::Create({Config::Instance()->video.frame_rate,
                                                                                      Config::Instance()->video.width,
                                                                                      Config::Instance()->video.height,
                                                                                      Config::Instance()->video.codec_type,
                                                                                      Config::Instance()->video.codec_mode,
                                                                                      Config::Instance()->video.codec_profile,
                                                                                      Config::Instance()->video.codec_bitrate});
    NVR_CHECK(NULL != video_codec_module);

    log_i("binding video process and video encode...");
    code = static_cast<err_code>(System::VPSSBindVENC());
    CHACK_ERROR(code)

    while (1)
        sleep(10);
    return 0;
}