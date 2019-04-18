#ifndef SYSTEM_H_
#define SYSTEM_H_

namespace nvr
{
struct System
{
    static int32_t InitMPP();
    static void InitLogger();

    static inline int32_t Align(int num, int align)
    {
        return (num + align - 1) & ~(align - 1);
    }

    static int32_t CalcPicVbBlkSize(int align = ALIGN);

    static uint64_t GetSteadyMicroSeconds();

    static int32_t VIBindVPSS();

    static int32_t VIUnBindVPSS();

    // static int32_t VPSSBindVENC();

    // static int32_t VPSSUnBindVENC();
};
} // namespace nvr
#endif