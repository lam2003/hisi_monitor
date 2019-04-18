#include "common/system.h"
#include "common/res_code.h"
#include "video_capture/video_capture_impl.h"
#include "video_process/video_process_impl.h"

#include "video_process/osd.h"

using namespace nvr;

int main(int argc, char **argv)
{
    err_code code;
    uint64_t start_time, end_time;

    //初始化日志系统
    System::InitLogger();

    //初始化海思sdk
    log_i("initializing mpp...");

    start_time = System::GetSteadyMicroSeconds();

    code = static_cast<err_code>(System::InitMPP());
    if (KSuccess != code)
    {
        log_e("error:%s", make_error_code(code).message().c_str());
        return static_cast<int>(code);
    }

    end_time = System::GetSteadyMicroSeconds();

    log_i("mpp initialize succeed,cost %lu us", end_time - start_time);

    //初始化视频采集模块
    log_i("initializing video capture...");

    start_time = System::GetSteadyMicroSeconds();

    rtc::scoped_refptr<VideoCaptureModule> video_capture_module = VideoCaptureImpl::Create();
    NVR_CHECK(NULL != video_capture_module)

    end_time = System::GetSteadyMicroSeconds();

    log_i("video capture initialize succeed,cost %lu us", end_time - start_time);

    //初始化视频处理模块
    log_i("initializing video process...");

    start_time = System::GetSteadyMicroSeconds();

    rtc::scoped_refptr<VideoProcessModule> video_process_module = VideoProcessImpl::Create();
    // NVR_CHECK(NULL != video_process_module)

    end_time = System::GetSteadyMicroSeconds();

    log_i("video capture process succeed,cost %lu us", end_time - start_time);

    while (1)
        sleep(10);

    // OSD::Instance()->Initialize();

    return 0;
}