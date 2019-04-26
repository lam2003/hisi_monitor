#ifndef RTMP_H_
#define RTMP_H_

#include "live/live.h"
#include "common/buffer.h"

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

extern "C"
{
#include "libavformat/avformat.h"
}

namespace nvr
{
//only support h264
class RtmpLiveImpl : public LiveModule
{
public:
    static rtc::scoped_refptr<LiveModule> Create(const Params &params);

    static int32_t ReadBuffer(void *opaque, uint8_t *buf, int requested_len);

    int32_t Initialize(const Params &params) override;

    void Close() override;

    void OnFrame(const VideoFrame &frame) override;

protected:
    RtmpLiveImpl();

    ~RtmpLiveImpl() override;

private:
    Buffer<> buffer_;
    std::mutex mux_;
    std::condition_variable cond_;
    bool run_;
    std::unique_ptr<std::thread> thread_;
    bool init_;
};
} // namespace nvr

#endif