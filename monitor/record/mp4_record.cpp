#include "record/mp4_record.h"
#include "record/mp4_muxer.h"
#include "common/res_code.h"
#include "common/system.h"
#include "base/ref_counted_object.h"

#include <sstream>

namespace nvr
{

rtc::scoped_refptr<RecordModule> MP4RecordImpl::Create(const Params &params)
{
    err_code code;

    rtc::scoped_refptr<MP4RecordImpl> implemention = new rtc::RefCountedObject<MP4RecordImpl>();

    code = static_cast<err_code>(implemention->Initialize(params));

    if (KSuccess != code)
    {
        log_e("error:%s", make_error_code(code).message().c_str());
        return nullptr;
    }

    return implemention;
}

bool MP4RecordImpl::RecordNeedToQuit()
{

    if (params_.use_md && System::GetSteadyMilliSeconds() >= end_time_)
        return true;
    return false;
}

bool MP4RecordImpl::RecordNeedToSegment(uint64_t start_time)
{
    if (System::GetSteadyMilliSeconds() > start_time + (params_.segment_duration * 1000))
        return true;
    return false;
}

void MP4RecordImpl::OnTrigger(int32_t num)
{
    end_time_ = System::GetSteadyMilliSeconds() + (params_.md_duration * 1000);
}

void MP4RecordImpl::RecordThread()
{
    err_code code;
    MP4Muxer muxer;
    VideoFrame frame;
    uint64_t now;
    uint64_t start_time;
    bool init = false;

    now = System::GetSteadyMilliSeconds();
    while (run_ && RecordNeedToQuit())
        usleep(500000); //500ms

    while (run_)
    {
        if (!init)
        {
            std::string path;
            {
                //创建文件夹,按日期创建
                std::ostringstream oss;
                oss << params_.path << '/' << System::GetLocalTime(RECORD_DIR_FORMAT);
                path = oss.str();
                code = static_cast<err_code>(System::CreateDir(path));
                if (KSuccess != code)
                    return;
            }

            std::ostringstream oss;
            oss << path << '/' << "record_" << System::GetLocalTime(RECORD_FILE_FORMAT) << ".mp4";
            code = static_cast<err_code>(muxer.Initialize(oss.str().c_str(), params_.width, params_.height, params_.frame_rate));
            if (KSuccess != code)
            {
                log_e("error:%s", make_error_code(code).message().c_str());
                return;
            }

            start_time = System::GetSteadyMilliSeconds();

            mux_.lock();
            buffer_.Clear();
            mux_.unlock();
            init = true;
        }

        std::unique_lock<std::mutex> lock(mux_);
        while (run_ &&
               !RecordNeedToQuit() &&
               !RecordNeedToSegment(start_time) &&
               !buffer_.Get((uint8_t *)&frame, sizeof(frame)))
            cond_.wait(lock);

        if (run_ &&
            !RecordNeedToQuit() &&
            !RecordNeedToSegment(start_time))
        {
            frame.data = buffer_.GetCurrentPos();
            code = static_cast<err_code>(muxer.WriteVideoFrame(frame));
            if (KSuccess != code)
            {
                log_e("error:%s", make_error_code(code).message().c_str());
                return;
            }
            if (!buffer_.Consume(frame.len))
            {
                log_e("buffer rest data not enough");
                return;
            }
        }

        if (RecordNeedToQuit())
        {
            muxer.Close();
            init = false;
            while (run_ && RecordNeedToQuit())
                usleep(500000); //500ms
                }
        else if (RecordNeedToSegment(start_time))
        {
            muxer.Close();
            init = false;
        }
    }
    muxer.Close();
}

int32_t MP4RecordImpl::Initialize(const Params &params)
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    params_ = params;
    run_ = true;
    thread_ = std::unique_ptr<std::thread>(new std::thread([this]() {
        RecordThread();
    }));

    init_ = true;
    return static_cast<int>(KSuccess);
}

void MP4RecordImpl::OnFrame(const VideoFrame &frame)
{
    if (!init_)
        return;

    mux_.lock();
    if (buffer_.FreeSpace() < sizeof(frame) + frame.len)
    {
        mux_.unlock();
        return;
    }

    buffer_.Append((uint8_t *)&frame, sizeof(frame));
    buffer_.Append(frame.data, frame.len);
    cond_.notify_one();
    mux_.unlock();
}

void MP4RecordImpl::Close()
{
    if (!init_)
        return;

    run_ = false;
    cond_.notify_all();
    thread_->join();
    thread_.reset();
    thread_ = nullptr;

    init_ = false;
}

MP4RecordImpl::MP4RecordImpl() : end_time_(0),
                                 run_(false),
                                 thread_(nullptr),
                                 init_(false)
{
}

MP4RecordImpl::~MP4RecordImpl()
{
    Close();
}
}; // namespace nvr