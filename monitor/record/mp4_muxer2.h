#ifndef MP4_MUXER_H_2_
#define MP4_MUXER_H_2_

#include "video_codec/video_codec_define.h"

#include <mp4v2/mp4v2.h>

#include <string>

namespace nvr
{

class MP4Muxer2
{
public:
    MP4Muxer2();

    ~MP4Muxer2();

    int32_t Initialize(const std::string &filename, int width, int height, int frame_rate);

    int32_t WriteVideoFrame(const VideoFrame &frame);

    void Close();
private:
    int32_t WriteMetaData();

private:
    MP4FileHandle handle_;
    MP4TrackId track_;
    int width_;
    int height_;
    int frame_rate_;
    bool write_meta_;
    bool init_;

}; 
} // namespace nvr
#endif