#include "record/mp4_muxer2.h"
#include "common/res_code.h"

namespace nvr
{

int32_t MP4Muxer2::Initialize(const std::string &filename, int width, int height, int frame_rate)
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    handle_ = MP4Create(filename.c_str());
    if (handle_ == MP4_INVALID_FILE_HANDLE)
    {
        log_e("MP4Create failed");
        return static_cast<int>(KThirdPartyError);
    }

    width_ = width;
    height_ = height;
    frame_rate_ = frame_rate;

    init_ = true;

    return static_cast<int>(KSuccess);
}

int32_t MP4Muxer2::WriteVideoFrame(const VideoFrame &frame)
{
    if (!init_)
        return static_cast<int>(KUnInitialize);

    int32_t ret;

    if (!write_meta_ && frame.type == H264Frame::NaluType::SPS)
    {

        if (!MP4SetTimeScale(handle_, 900000))
        {
            log_e("MP4SetTimeScale failed");
            return static_cast<int>(KThirdPartyError);
        }

        track_ = MP4AddH264VideoTrack(handle_, 900000, 900000 / frame_rate_, width_, height_, frame.data[5], frame.data[6], frame.data[7], 3);
        if (track_ == MP4_INVALID_TRACK_ID)
        {
            log_e("MP4AddH264VideoTrack failed");
            return static_cast<int>(KThirdPartyError);
        }

        MP4SetVideoProfileLevel(handle_, 0x7F);

        write_meta_ = true;
    }

    if (write_meta_)
    {
        switch (frame.type)
        {
        case H264Frame::NaluType::SPS:
            MP4AddH264SequenceParameterSet(handle_, track_, &frame.data[4], frame.len - 4);
            break;

        case H264Frame::NaluType::PPS:
        case H264Frame::NaluType::SEI:
            MP4AddH264PictureParameterSet(handle_, track_, &frame.data[4], frame.len - 4);
            break;
        case H264Frame::NaluType::ISLICE:
        case H264Frame::NaluType::PSLICE:
        {
            uint32_t len = frame.len - 4;
            frame.data[0] = (len >> 24) & 0xff;
            frame.data[1] = (len >> 16) & 0xff;
            frame.data[2] = (len >> 8) & 0xff;
            frame.data[3] = len & 0xff;

            ret = MP4WriteSample(handle_, track_, frame.data, frame.len, MP4_INVALID_DURATION);
            if (!ret)
            {
                log_e("MP4WriteSample failed");
                return static_cast<int>(KThirdPartyError);
            }
            break;
        }

        default:
            log_w("unknow h264 frame type:%d", frame.type);
            break;
        }
    }

    return static_cast<int>(KSuccess);
}

void MP4Muxer2::Close()
{
    if (!init_)
        return;

    MP4Close(handle_);
    handle_ = MP4_INVALID_FILE_HANDLE;
    track_ = MP4_INVALID_TRACK_ID;
    width_ = 0;
    height_ = 0;
    frame_rate_ = 0;
    write_meta_ = false;

    init_ = false;
}

MP4Muxer2::MP4Muxer2() : handle_(MP4_INVALID_FILE_HANDLE),
                         track_(MP4_INVALID_TRACK_ID),
                         width_(0),
                         height_(0),
                         frame_rate_(0),
                         write_meta_(false),
                         init_(false)
{
}

MP4Muxer2::~MP4Muxer2()
{
    Close();
}
}; // namespace nvr