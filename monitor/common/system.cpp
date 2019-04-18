#include "common/system.h"
#include "common/res_code.h"

#include <chrono>

namespace nvr
{

int32_t System::InitMPP()
{
    int32_t ret;
    int32_t pic_vb_blk_size = CalcPicVbBlkSize();

    VB_CONF_S vb_cfg;
    memset(&vb_cfg, 0, sizeof(VB_CONF_S));
    vb_cfg.u32MaxPoolCnt = VB_POOLS_NUM;
    vb_cfg.astCommPool[0].u32BlkSize = pic_vb_blk_size;
    vb_cfg.astCommPool[0].u32BlkCnt = VB_MEM_BLK_NUM;

    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();

    ret = HI_MPI_VB_SetConf(&vb_cfg);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VB_SetConf failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_VB_Init();
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VB_Init failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    MPP_SYS_CONF_S sys_conf;
    memset(&sys_conf, 0, sizeof(sys_conf));
    sys_conf.u32AlignWidth = ALIGN;

    ret = HI_MPI_SYS_SetConf(&sys_conf);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_SYS_SetConf failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_SYS_Init();
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_SYS_Init failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
}

void System::InitLogger()
{
    setbuf(stdout, NULL);
    elog_init();
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_text_color_enabled(true);
    elog_start();
}

int32_t System::CalcPicVbBlkSize(int align)
{
    int32_t vb_pic_header_size;
    int32_t align_width = Align(PIC_WIDTH, align);
    int32_t align_height = Align(PIC_HEIGHT, align);
    VB_PIC_HEADER_SIZE(PIC_WIDTH, PIC_HEIGHT, PIXEL_FORMAT, vb_pic_header_size);
    return vb_pic_header_size + ((align_width * align_height) * 3 >> 1);
}

uint64_t System::GetSteadyMicroSeconds()
{
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto now_since_epoch = now.time_since_epoch();
    return duration_cast<microseconds>(now_since_epoch).count();
}

int32_t VIUnBindVPSS()
{
    int32_t ret;

    MPP_CHN_S src_chn;
    src_chn.enModId = HI_ID_VIU;
    src_chn.s32DevId = NVR_VI_DEV;
    src_chn.s32ChnId = NVR_VI_CHN;

    MPP_CHN_S dest_chn;
    dest_chn.enModId = HI_ID_VENC;
    dest_chn.s32DevId = NVR_VPSS_GRP;
    dest_chn.s32ChnId = NVR_VPSS_CHN;

    ret = HI_MPI_SYS_UnBind(&src_chn, &dest_chn);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_SYS_UnBind failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
}

int32_t VIBindVPSS()
{
    int32_t ret;

    MPP_CHN_S src_chn;
    src_chn.enModId = HI_ID_VIU;
    src_chn.s32DevId = NVR_VI_DEV;
    src_chn.s32ChnId = NVR_VI_CHN;

    MPP_CHN_S dest_chn;
    dest_chn.enModId = HI_ID_VENC;
    dest_chn.s32DevId = NVR_VPSS_GRP;
    dest_chn.s32ChnId = NVR_VPSS_CHN;

    ret = HI_MPI_SYS_Bind(&src_chn, &dest_chn);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_SYS_Bind failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
}
} // namespace nvr
