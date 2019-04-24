#ifndef VIDEO_CAPTRUE_MODULE_H_
#define VIDEO_CAPTURE_MODULE_H_

#include "base/scoped_refptr.h"
#include "base/ref_count.h"

namespace nvr
{

class VideoCaptureModule : public rtc::RefCountInterface
{
public:

    virtual int32_t Initialize() = 0;

    virtual void Close() = 0;

protected:
    ~VideoCaptureModule() override {}

};

} // namespace nvr

#endif