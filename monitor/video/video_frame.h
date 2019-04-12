#ifndef VIDEO_FRAME_H_
#define VIDEO_FRAME_H_

namespace nvr
{
struct VideoFrame
{
    enum class Type
    {
        KNative,
        KI420,
    };

    int width;
    int height;
    int stride_y;
    int stride_u;
    int stride_v;

    uint8_t *data;
    uint8_t *data_y;
    uint8_t *data_u;
    uint8_t *data_v;

    uint64_t timestamp;
};

}; // namespace nvr

#endif