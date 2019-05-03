#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "global.h"

#include <string>

namespace nvr
{
struct System
{
    static int32_t InitMPP();

    static void UnInitMPP();

    static void InitLogger();

    static inline int32_t Align(int num, int align = ALIGN)
    {
        return (num + align - 1) & ~(align - 1);
    }

    static int32_t CalcPicVbBlkSize(int width, int height, int align = ALIGN);

    static uint64_t GetSteadyMilliSeconds();

    static int32_t VIBindVPSS();

    static int32_t VIUnBindVPSS();

    static int32_t VPSSBindVENC();

    static int32_t VPSSUnBindVENC();

    static int32_t CreateDir(const std::string &path);

    static std::string GetLocalTime(const std::string &format);
};
} // namespace nvr
#endif