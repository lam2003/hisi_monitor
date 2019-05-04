#include "live/rtmp.h"
#include "common/res_code.h"
#include "live/rtmp_streamer.h"

#include <base/ref_counted_object.h>

namespace nvr
{

rtc::scoped_refptr<LiveModule> RtmpLiveImpl::Create(const Params &params)
{
    err_code code;

    rtc::scoped_refptr<RtmpLiveImpl> implemention = new rtc::RefCountedObject<RtmpLiveImpl>();

    code = static_cast<err_code>(implemention->Initialize(params));

    if (KSuccess != code)
    {
        log_e("error:%s", make_error_code(code).message().c_str());
        return nullptr;
    }

    return implemention;
}

int32_t RtmpLiveImpl::Initialize(const Params &params)
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    run_ = true;
    thread_ = std::unique_ptr<std::thread>(new std::thread([this, params]() {
        err_code code;
        RTMPStreamer rtmp_streamer;
        VideoFrame frame;

        bool init = false;

        while (run_)
        {

            if (!init)
            {
                code = static_cast<err_code>(rtmp_streamer.Initialize(params.url));
                if (KSuccess != code)
                {
                    log_e("error:%s", make_error_code(code).message().c_str());
                    return;
                }
                init = true;
            }

            std::unique_lock<std::mutex> lock(mux_);
            while (run_ && !buffer_.Get((uint8_t *)&frame, sizeof(frame)))
                cond_.wait(lock);

            if (run_)
            {
                frame.data = buffer_.GetCurrentPos();
                code = static_cast<err_code>(rtmp_streamer.WriteVideoFrame(frame));
                if (KSuccess != code)
                {
                    log_w("rtmp connection break,try to reconnect...");
                    rtmp_streamer.Close();
                    buffer_.Clear();
                    init = false;
                    continue;
                }
                if (!buffer_.Consume(frame.len))
                {
                    log_e("consme data from buffer failed,rest data not enough");
                    return;
                }
            }
        }

        rtmp_streamer.Close();
    }));

    init_ = true;
    return static_cast<int>(KSuccess);
}

void RtmpLiveImpl::OnFrame(const VideoFrame &frame)
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

void RtmpLiveImpl::Close()
{
    if (!init_)
        return;

    run_ = false;
    cond_.notify_all();
    thread_->join();
    thread_.reset();
    thread_ = nullptr;
    buffer_.Clear();
    init_ = false;
}

RtmpLiveImpl::RtmpLiveImpl() : run_(false),
                               thread_(nullptr),
                               init_(false)
{
}

RtmpLiveImpl::~RtmpLiveImpl()
{
    Close();
}
}; // namespace nvr