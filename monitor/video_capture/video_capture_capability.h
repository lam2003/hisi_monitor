#ifndef VIDEO_CAPTURE_CAPABILITY_H_
#define VIDEO_CAPTURE_CAPABILITY_H_

#include "video/video_frame.h"
#include "video/video_rotation.h"

namespace nvr
{
struct VideoCaptureCapability
{
    int32_t width;
    int32_t height;
    int32_t max_fps;
    VideoFrame::Type type;
    bool interlaced;
    VideoRotation rotation;

    VideoCaptureCapability()
    {
        width = 0;
        height = 0;
        max_fps = 0;
        type = VideoFrame::Type::KNative;
        interlaced = false;
        rotation = VideoRotation::kVideoRotation_0;
    }
};
} // namespace nvr

#endif