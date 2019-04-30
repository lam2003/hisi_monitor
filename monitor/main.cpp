#include "common/system.h"
#include "common/res_code.h"
#include "common/config.h"
#include "video_capture/video_capture_impl.h"
#include "video_process/video_process_impl.h"
#include "video_detect/video_detect_impl.h"
#include "video_codec/video_codec_impl.h"
#include "live/rtmp.h"
#include "record/mp4_record.h"

using namespace nvr;

static bool KRun = true;

void signal_handler(int signo)
{
    if (signo == SIGINT)
    {
        log_w("recevice SIGINT,quit process");
        KRun = false;
    }
    else if (signo == SIGPIPE)
    {
        log_w("recevice SIGPIPE");
    }
}

int main(int argc, char **argv)
{
    err_code code;
    uint64_t start_time, end_time;

    //初始化日志系统
    System::InitLogger();

    //初始化信号处理函数
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, signal_handler);

    //初始化FFMPEG
    System::InitFFMPEG();

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
                                                                                            Config::Instance()->video.height,
                                                                                            Config::Instance()->detect.width,
                                                                                            Config::Instance()->detect.height});
    NVR_CHECK(NULL != video_process_module)

    log_i("binding video capture and video process...");
    code = static_cast<err_code>(System::VIBindVPSS());
    CHACK_ERROR(code)

    //初始化运动侦测模块
    log_i("initializing video detect...");
    rtc::scoped_refptr<VideoDetectModule> video_detect_module = VideoDetectImpl::Create({Config::Instance()->detect.width,
                                                                                         Config::Instance()->detect.height,
                                                                                         Config::Instance()->detect.trigger_thresh});
    NVR_CHECK(NULL != video_process_module)

    log_i("attach video detect to video process...");
    video_process_module->SetVideoSink(video_detect_module);

    //初始化视频编码模块
    log_i("initializing video encode...");
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

    // //初始化直播
    log_i("initializing live...");
    rtc::scoped_refptr<LiveModule> live_module = RtmpLiveImpl::Create({Config::Instance()->rtmp.url});
    NVR_CHECK(NULL != live_module);

    log_i("attach live to video encode...");
    video_codec_module->AddVideoSink(live_module);

    // log_i("initializing record...");
    // rtc::scoped_refptr<RecordModule> record_module = MP4RecordImpl::Create({Config::Instance()->video.frame_rate,
    //                                                                         Config::Instance()->video.width,
    //                                                                         Config::Instance()->video.height,
    //                                                                         Config::Instance()->video.codec_type,
    //                                                                         "1997.mp4"});
    // NVR_CHECK(NULL != record_module);

    // log_i("attach record to video encode...");
    // video_codec_module->AddVideoSink(record_module);

    while (KRun)
        sleep(1000);

    log_i("detch live/record and video encode...");
    video_codec_module->ClearVideoSink();

    // log_i("closing record...");
    // record_module->Close();

    log_i("closing live...");
    live_module->Close();

    log_i("unbinding video process and video encode...");
    System::VPSSUnBindVENC();

    log_i("closing video encode...");
    video_codec_module->Close();

    log_i("detach video detect and video process...");
    video_process_module->SetVideoSink(nullptr);

    log_i("closing video detect...");
    video_process_module->Close();

    log_i("unbinding video capture and video process...");
    System::VIUnBindVPSS();

    log_i("closing video process...");
    video_process_module->Close();

    log_i("closing video capture...");
    video_capture_module->Close();

    log_i("closing mpp...");
    System::UnInitMPP();

    return 0;
}