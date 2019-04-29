#ifndef VIDEO_FRAME_H_
#define VIDEO_FRAME_H_

namespace nvr
{

struct VideoFrame
{
  virtual ~VideoFrame() = default;

  virtual int32_t GetCodecType() const { return 0; }

  uint8_t *data;

  uint32_t len;

  uint64_t ts;

  int32_t type;
};

} // namespace nvr

#endif