#ifndef MP4_RECORD_H_
#define MP4_RECORD_H_

#include "record/record.h"
#include "common/buffer.h"

#include <memory>
#include <thread>
#include <mutex>
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

protected:
    MP4RecordImpl();

    ~MP4RecordImpl() override;

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