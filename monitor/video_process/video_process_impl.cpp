#include "video_process_impl.h"
#include "base/ref_counted_object.h"
#include "common/res_code.h"

namespace nvr
{

rtc::scoped_refptr<VideoProcessModule> VideoProcessImpl::Create()
{
    err_code code;

    rtc::scoped_refptr<VideoProcessImpl> implemention(new rtc::RefCountedObject<VideoProcessImpl>());

    code = static_cast<err_code>(implemention->Initialize());

    if (KSuccess != code)
    {
        log_e("error:%s", make_error_code(code).message().c_str());
        return nullptr;
    }

    return implemention;
}

VideoProcessImpl::VideoProcessImpl() : init_(false)
{
}

VideoProcessImpl::~VideoProcessImpl()
{
}

int32_t VideoProcessImpl::SetRotate(ROTATE_E rotate)
{
    return static_cast<int>(KSuccess);
}

int32_t VideoProcessImpl::SetFrameRate(int frame_rate)
{
    return static_cast<int>(KSuccess);
}

int32_t VideoProcessImpl::SetMirror(bool mirror)
{
    return static_cast<int>(KSuccess);
}

int32_t VideoProcessImpl::SetFlip(bool flip)
{
    return static_cast<int>(KSuccess);
}

int32_t VideoProcessImpl::setScale(const SIZE_S &size)
{
    return static_cast<int>(KSuccess);
}

int32_t VideoProcessImpl::StartVPSSGroup()
{
    int32_t ret;

    VPSS_GRP_ATTR_S vpss_grp_attr;
    memset(&vpss_grp_attr, 0, sizeof(vpss_grp_attr));

    vpss_grp_attr.u32MaxW = PIC_WIDTH;
    vpss_grp_attr.u32MaxH = PIC_HEIGHT;
    vpss_grp_attr.bIeEn = HI_FALSE;
    vpss_grp_attr.bNrEn = HI_TRUE;
    vpss_grp_attr.bHistEn = HI_FALSE;
    vpss_grp_attr.bDciEn = HI_FALSE;
    vpss_grp_attr.enDieMode = VPSS_DIE_MODE_NODIE;
    vpss_grp_attr.enPixFmt = PIXEL_FORMAT;
    vpss_grp_attr.bSharpenEn = HI_FALSE;

    ret = HI_MPI_VPSS_CreateGrp(NVR_VPSS_GRP, &vpss_grp_attr);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VPSS_CreateGrp failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_VPSS_StartGrp(NVR_VPSS_GRP);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VPSS_StartGrp failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
}

int32_t VideoProcessImpl::StartVPSSChn()
{
    int32_t ret;

    VPSS_CHN_ATTR_S vpss_chn_attr;
    memset(&vpss_chn_attr, 0, sizeof(vpss_chn_attr));
    vpss_chn_attr.s32SrcFrameRate = -1;
    vpss_chn_attr.s32DstFrameRate = -1;

    ret = HI_MPI_VPSS_SetChnAttr(NVR_VPSS_GRP, NVR_VPSS_CHN, &vpss_chn_attr);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VPSS_SetChnAttr failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    VPSS_CHN_MODE_S vpss_chn_mode;
    memset(&vpss_chn_mode, 0, sizeof(vpss_chn_mode));
    vpss_chn_mode.enChnMode = VPSS_CHN_MODE_USER;
    vpss_chn_mode.bDouble = HI_FALSE;
    vpss_chn_mode.enPixelFormat = PIXEL_FORMAT;
    vpss_chn_mode.u32Width = PIC_WIDTH;
    vpss_chn_mode.u32Height = PIC_HEIGHT;
    vpss_chn_mode.enCompressMode = COMPRESS_MODE_NONE;

    ret = HI_MPI_VPSS_SetChnMode(NVR_VPSS_GRP, NVR_VPSS_CHN, &vpss_chn_mode);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VPSS_SetChnMode failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_VPSS_EnableChn(NVR_VPSS_GRP, NVR_VPSS_CHN);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VPSS_EnableChn failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
} // namespace nvr

int32_t VideoProcessImpl::Initialize()
{
    if (init_)
        return static_cast<int>(KDupInitialize);
    err_code code;

    code = static_cast<err_code>(StartVPSSGroup());
    if (KSuccess != code)
        return static_cast<int>(code);

    code = static_cast<err_code>(StartVPSSChn());
    if (KSuccess != code)
        return static_cast<int>(code);

    init_ = true;

    return static_cast<int>(KSuccess);
}

void VideoProcessImpl::close()
{
}
}; // namespace nvr