#ifndef MP4_RECORD_H_
#define MP4_RECORD_H_

#include "record/record.h"
#include "common/buffer.h"

#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace nvr
{
class MP4RecordImpl : public RecordModule
{
public:
    static rtc::scoped_refptr<RecordModule> Create(const Params &params);

    int32_t Initialize(const Params &params) override;

    void Close() override;

    void OnFrame(const VideoFrame &) override;

    void OnTrigger(int32_t num) override;

protected:
    MP4RecordImpl();

    ~MP4RecordImpl() override;

private:
    void RecordThread();
    bool RecordNeedToQuit();
    bool RecordNeedToSegment(uint64_t start_time);

private:
    std::mutex mux_;
    std::condition_variable cond_;
    Buffer<> buffer_;
    Params params_;
    std::atomic<uint64_t> end_time_;
    bool run_;
    std::unique_ptr<std::thread> thread_;
    bool init_;
};

} // namespace nvr

#endif