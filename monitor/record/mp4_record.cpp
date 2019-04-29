#include "record/mp4_record.h"
#include "record/mp4_muxer.h"
#include "common/res_code.h"
#include "base/ref_counted_object.h"

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

int32_t MP4RecordImpl::Initialize(const Params &params)
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    run_ = true;
    thread_ = std::unique_ptr<std::thread>(new std::thread([this, params]() {
        err_code code;
        MP4Muxer muxer;
        VideoFrame frame;

        bool init = false;
        while (run_)
        {
            if (!init)
            {
                code = static_cast<err_code>(muxer.Initialize(params.filename, params.width, params.height, params.frame_rate, params.codec_type));
                if (KSuccess != code)
                {
                    log_e("error:%s", make_error_code(code).message().c_str());
                    return;
                }
                init = true;
            }

            std::unique_lock<std::mutex> lock(mux_);
            while (!buffer_.Get((uint8_t *)&frame, sizeof(frame)))
            {
                cond_.wait(lock);
                if (!run_)
                    break;
            }
            printf("get %d\n", sizeof(frame));
            printf("get %d\n", frame.len);

            if (run_)
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
        }
        muxer.Close();
    }));

    init_ = true;
    return static_cast<int>(KSuccess);
}

void MP4RecordImpl::OnFrame(const VideoFrame &frame)
{
    if (!init_)
        return;

    std::unique_lock<std::mutex> lock(mux_);
    if (buffer_.FreeSpace() < sizeof(frame) + frame.len)
        return;

    buffer_.Append((uint8_t *)&frame, sizeof(frame));
    printf("append %d\n", sizeof(frame));
    buffer_.Append(frame.data, frame.len);
    printf("append %d\n", frame.len);
    cond_.notify_one();
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

MP4RecordImpl::MP4RecordImpl() : run_(false),
                                 thread_(nullptr),
                                 init_(false)
{
}

MP4RecordImpl::~MP4RecordImpl()
{
    Close();
}
}; // namespace nvr