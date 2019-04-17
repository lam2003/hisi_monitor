#include "video/video_sink_interface.h"
#include "base/ref_count.h"

namespace nvr
{
class VideoProcessModule : public rtc::RefCountInterface, public VideoSinkInterface<VIDEO_FRAME_INFO_S>
{
public:
    VideoProcessModule();

    virtual void OnFrame(const VIDEO_FRAME_INFO_S &frame) = 0;

    virtual int32_t SetRotate(ROTATE_E rotate) = 0;

    virtual int32_t SetFrameRate(int frame_rate) = 0;

    virtual int32_t SetMirror(bool mirror) = 0;

    virtual int32_t SetFlip(bool flip) = 0;

    virtual int32_t setScale(const SIZE_S &size) = 0;

    virtual int32_t StartProcess() = 0;

    virtual int32_t StopProcess() = 0;

protected:
    ~VideoProcessModule() override{};

private:
    std::thread process_thread_;
    bool run_;
    VideoSinkInterface<VIDEO_FRAME_INFO_S> *video_sink_;
    bool init_;
};
}; // namespace nvr