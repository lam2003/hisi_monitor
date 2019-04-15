

#include "common/config.h"
#include "common/utils.h"

using namespace nvr;

void init_log()
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

int32_t init_hisi_mmp()
{
    int32_t ret;
    int32_t pic_vb_blk_size = Utils::CalcPicVbBlkSize();
    log_d("[pic_vb_blk_size]:%d bytes", pic_vb_blk_size);

    VB_CONF_S vb_cfg;
    memset(&vb_cfg, 0, sizeof(VB_CONF_S));
    vb_cfg.u32MaxPoolCnt = Config::Instance()->system.vb_pools_num;
    vb_cfg.astCommPool[0].u32BlkSize = pic_vb_blk_size;
    vb_cfg.astCommPool[0].u32BlkCnt = 10;

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

int main(int argc, char **argv)
{

    //初始化日志系统
    init_log();
    // 初始化海思sdk
    log_d("initializing hisi mpp...");

    sleep(1);
    return 0;
}