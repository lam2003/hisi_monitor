#include "common/system.h"
#include "common/res_code.h"

#include <chrono>

namespace nvr
{

int32_t System::InitMPP()
{
    int32_t ret;

    VB_CONF_S vb_cfg;
    memset(&vb_cfg, 0, sizeof(VB_CONF_S));
    vb_cfg.u32MaxPoolCnt = VB_POOLS_NUM;
    vb_cfg.astCommPool[0].u32BlkSize = CalcPicVbBlkSize(PIC_WIDTH, PIC_HEIGHT);
    vb_cfg.astCommPool[0].u32BlkCnt = VB_MEM_BLK_NUM;
    vb_cfg.astCommPool[1].u32BlkSize = CalcPicVbBlkSize(DETECT_WIDTH, DETECT_HEIGHT);
    vb_cfg.astCommPool[1].u32BlkCnt = DETECT_MEM_BLK_NUM;

    ret = HI_MPI_SYS_Exit();
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_SYS_Exit failed,code %#x", ret);

    ret = HI_MPI_VB_Exit();
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_VB_Exit failed,code %#x", ret);

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

void System::UnInitMPP()
{
    int32_t ret;

    ret = HI_MPI_SYS_Exit();
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_SYS_Exit failed,code %#x", ret);

    ret = HI_MPI_VB_Exit();
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_VB_Exit failed,code %#x", ret);
}

void System::InitLogger()
{
    setbuf(stdout, NULL);
    elog_init();
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_T_INFO | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_T_INFO | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_text_color_enabled(true);
    elog_start();
}

int32_t System::CalcPicVbBlkSize(int width, int height, int align)
{
    int32_t vb_pic_header_size;
    int32_t align_width = Align(width, align);
    int32_t align_height = Align(height, align);
    VB_PIC_HEADER_SIZE(width, height, PIXEL_FORMAT, vb_pic_header_size);
    return vb_pic_header_size + ((align_width * align_height) * 3 >> 1);
}

uint64_t System::GetSteadyMilliSeconds()
{
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto now_since_epoch = now.time_since_epoch();
    return duration_cast<milliseconds>(now_since_epoch).count();
}

int32_t System::VIUnBindVPSS()
{
    int32_t ret;

    MPP_CHN_S src_chn;
    src_chn.enModId = HI_ID_VIU;
    src_chn.s32DevId = NVR_VI_DEV;
    src_chn.s32ChnId = NVR_VI_CHN;

    MPP_CHN_S dest_chn;
    dest_chn.enModId = HI_ID_VPSS;
    dest_chn.s32DevId = NVR_VPSS_GRP;
    dest_chn.s32ChnId = 0;

    ret = HI_MPI_SYS_UnBind(&src_chn, &dest_chn);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_SYS_UnBind failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
}

int32_t System::VIBindVPSS()
{
    int32_t ret;

    MPP_CHN_S src_chn;
    src_chn.enModId = HI_ID_VIU;
    src_chn.s32DevId = NVR_VI_DEV;
    src_chn.s32ChnId = NVR_VI_CHN;

    MPP_CHN_S dest_chn;
    dest_chn.enModId = HI_ID_VPSS;
    dest_chn.s32DevId = NVR_VPSS_GRP;
    dest_chn.s32ChnId = 0;

    ret = HI_MPI_SYS_Bind(&src_chn, &dest_chn);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_SYS_Bind failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
}

int32_t System::VPSSUnBindVENC()
{
    int32_t ret;

    MPP_CHN_S src_chn;
    src_chn.enModId = HI_ID_VPSS;
    src_chn.s32DevId = NVR_VPSS_GRP;
    src_chn.s32ChnId = NVR_VPSS_ENCODE_CHN;

    MPP_CHN_S dest_chn;
    dest_chn.enModId = HI_ID_VENC;
    dest_chn.s32DevId = 0;
    dest_chn.s32ChnId = NVR_VENC_CHN;

    ret = HI_MPI_SYS_UnBind(&src_chn, &dest_chn);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_SYS_UnBind failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
}

int32_t System::VPSSBindVENC()
{
    int32_t ret;

    MPP_CHN_S src_chn;
    src_chn.enModId = HI_ID_VPSS;
    src_chn.s32DevId = NVR_VPSS_GRP;
    src_chn.s32ChnId = NVR_VPSS_ENCODE_CHN;

    MPP_CHN_S dest_chn;
    dest_chn.enModId = HI_ID_VENC;
    dest_chn.s32DevId = 0;
    dest_chn.s32ChnId = NVR_VENC_CHN;

    ret = HI_MPI_SYS_Bind(&src_chn, &dest_chn);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_SYS_Bind failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
}

int32_t System::CreateDir(const std::string &path)
{
    size_t pos = 0;
    while (true)
    {
        pos = path.find_first_of('/', pos);
        std::string sub_str = path.substr(0, pos);
        if (sub_str != "" && access(sub_str.c_str(), F_OK) != 0)
        {
            log_w("create dir %s", sub_str.c_str());
            if (mkdir(sub_str.c_str(), 0777) != 0)
            {
                log_e("mkdir failed,%s", strerror(errno));
                return static_cast<int>(KSystemError);
            }
        }
        if (pos == std::string::npos)
            break;
        pos++;
    }

    return static_cast<int>(KSuccess);
}

std::string System::GetLocalTime(const std::string &format)
{
    time_t t = time(nullptr);
    char buf[256];
    strftime(buf, sizeof(buf), format.c_str(), localtime(&t));
    return std::string(buf, strlen(buf));
}

} // namespace nvr
