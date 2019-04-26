#include "live/rtmp.h"
#include "common/res_code.h"
#include "base/ref_counted_object.h"
#include "live/rtmp_streamer.h"

#define IO_BUFFER_SIZE (32 * 1024)

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

    NVR_CHECK(params.codec_type == H264);

    run_ = true;
    thread_ = std::unique_ptr<std::thread>(new std::thread([this, params]() {
        err_code code;
        RTMPStreamer rtmp_streamer;
        H264Frame frame;
        std::string sps, pps;
        uint8_t *buf;

        bool streamer_init = false;
        buf = (uint8_t *)malloc(512 * 1024);

        while (run_)
        {
            std::unique_lock<std::mutex> lock(mux_);

            while (!buffer_.Get((uint8_t *)&frame, sizeof(frame)))
            {
                cond_.wait(lock);
                if (!run_)
                    break;
            }

            frame.data = buffer_.GetCurrentPos();

            if (!streamer_init && (frame.type == (int)H264Frame::NaluType::SPS || frame.type == (int)H264Frame::NaluType::PPS))
            {
                if (frame.type == (int)H264Frame::NaluType::SPS)
                    sps = std::string((char *)frame.data, frame.len);
                else if (frame.type == (int)H264Frame::NaluType::PPS)
                    pps = std::string((char *)frame.data, frame.len);

                if (!sps.empty() && !pps.empty())
                {
                    code = static_cast<err_code>(rtmp_streamer.Initialize(params.url, params.width, params.height, params.frame_rate, sps, pps));
                    if (KSuccess != code)
                    {
                        log_e("error:%s", make_error_code(code).message().c_str());
                        return;
                    }

                    streamer_init = true;
                }
            }

            if (streamer_init)
            {
                code = static_cast<err_code>(rtmp_streamer.WriteVideoFrame(frame));
                if (KSuccess != code)
                {
                    rtmp_streamer.Close();
                    buffer_.Clear();
                    sps.clear();
                    pps.clear();
                    streamer_init = false;
                    log_w("rtmp connection break,try to reconnect...");
                }
            }
            buffer_.Consume(frame.len);
        }
    }));

    init_ = true;
    return static_cast<int>(KSuccess);
}

void RtmpLiveImpl::OnFrame(const VideoFrame &frame)
{
    if (!init_)
        return;

    NVR_CHECK(frame.GetCodecType() == H264);

    std::unique_lock<std::mutex>(mux_);

    if (buffer_.FreeSpace() < sizeof(frame) + frame.len)
        return;

    buffer_.Append((uint8_t *)&frame, sizeof(frame));
    buffer_.Append(frame.data, frame.len);
    cond_.notify_one();
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