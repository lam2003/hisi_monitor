#ifndef VIDEO_SINK_INTERFACE_H_
#define VIDEO_SINK_INTERFACE_H_

namespace nvr
{

template <typename VideoFrameT>
class VideoSinkInterface
{
public:
    virtual ~VideoSinkInterface() = default;

    virtual void onFrame(const VideoFrameT &frame) = 0;
};

} // namespace nvr

#endif
