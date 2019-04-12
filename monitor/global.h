#pragma once

#include <stdint.h>
#include <elog.h>

#define NVR_CHECK(condition) \
    while (!(condition))     \
    log_e("check %s failed", #condition)
