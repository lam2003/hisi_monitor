#include "video_process_impl.h"
#include "base/ref_counted_object.h"
#include "common/res_code.h"

namespace nvr
{

rtc::scoped_refptr<VideoProcessModule> VideoProcessImpl::Create(const Params &params)
{
    err_code code;

    rtc::scoped_refptr<VideoProcessImpl> implemention(new rtc::RefCountedObject<VideoProcessImpl>());

    code = static_cast<err_code>(implemention->Initialize(params));

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
    Close();
}

int32_t VideoProcessImpl::StartVPSSGroup(const Params &params)
{
    int32_t ret;

    VPSS_GRP_ATTR_S grp_attr;
    memset(&grp_attr, 0, sizeof(grp_attr));

    grp_attr.u32MaxW = PIC_WIDTH;
    grp_attr.u32MaxH = PIC_HEIGHT;
    grp_attr.bIeEn = HI_FALSE;
    grp_attr.bNrEn = HI_TRUE;
    grp_attr.bHistEn = HI_FALSE;
    grp_attr.bDciEn = HI_FALSE;
    grp_attr.enDieMode = VPSS_DIE_MODE_NODIE;
    grp_attr.enPixFmt = PIXEL_FORMAT;
    grp_attr.bSharpenEn = HI_FALSE;

    ret = HI_MPI_VPSS_CreateGrp(NVR_VPSS_GRP, &grp_attr);
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

void VideoProcessImpl::StopVPSSGroup()
{
    int32_t ret;

    ret = HI_MPI_VPSS_StopGrp(NVR_VPSS_GRP);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_VPSS_StopGrp failed,code %#x", ret);

    ret = HI_MPI_VPSS_DestroyGrp(NVR_VPSS_GRP);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_VPSS_DestroyGrp failed,code %#x", ret);
}

int32_t VideoProcessImpl::StartVPSSEncodeChn(const Params &params)
{
    int32_t ret;

    VPSS_CHN_ATTR_S chn_attr;
    memset(&chn_attr, 0, sizeof(chn_attr));
    chn_attr.s32SrcFrameRate = FRAME_RATE;
    chn_attr.s32DstFrameRate = params.frame_rate;

    ret = HI_MPI_VPSS_SetChnAttr(NVR_VPSS_GRP, NVR_VPSS_ENCODE_CHN, &chn_attr);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VPSS_SetChnAttr failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    VPSS_CHN_MODE_S chn_mode;
    memset(&chn_mode, 0, sizeof(chn_mode));
    chn_mode.enChnMode = VPSS_CHN_MODE_USER;
    chn_mode.bDouble = HI_FALSE;
    chn_mode.enPixelFormat = PIXEL_FORMAT;
    chn_mode.u32Width = params.encode_width;
    chn_mode.u32Height = params.encode_height;
    chn_mode.enCompressMode = COMPRESS_MODE_SEG;

    ret = HI_MPI_VPSS_SetChnMode(NVR_VPSS_GRP, NVR_VPSS_ENCODE_CHN, &chn_mode);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VPSS_SetChnMode failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_VPSS_EnableChn(NVR_VPSS_GRP, NVR_VPSS_ENCODE_CHN);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VPSS_EnableChn failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
}

void VideoProcessImpl::StopVPSSEncodeChn()
{
    int32_t ret;

    ret = HI_MPI_VPSS_DisableChn(NVR_VPSS_GRP, NVR_VPSS_ENCODE_CHN);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_VPSS_DisableChn failed,code %#x", ret);
}

int32_t VideoProcessImpl::StartVPSSDetectChn(const Params &params)
{
    int32_t ret;

    VPSS_CHN_ATTR_S chn_attr;
    memset(&chn_attr, 0, sizeof(chn_attr));
    chn_attr.s32SrcFrameRate = FRAME_RATE;
    chn_attr.s32DstFrameRate = params.frame_rate;

    ret = HI_MPI_VPSS_SetChnAttr(NVR_VPSS_GRP, NVR_VPSS_DETECT_CHN, &chn_attr);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VPSS_SetChnAttr failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    VPSS_CHN_MODE_S chn_mode;
    memset(&chn_mode, 0, sizeof(chn_mode));
    chn_mode.enChnMode = VPSS_CHN_MODE_USER;
    chn_mode.bDouble = HI_FALSE;
    chn_mode.enPixelFormat = PIXEL_FORMAT;
    chn_mode.u32Width = params.detect_width;
    chn_mode.u32Height = params.detect_height;
    chn_mode.enCompressMode = COMPRESS_MODE_NONE;

    ret = HI_MPI_VPSS_SetChnMode(NVR_VPSS_GRP, NVR_VPSS_DETECT_CHN, &chn_mode);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VPSS_SetChnMode failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_VPSS_EnableChn(NVR_VPSS_GRP, NVR_VPSS_DETECT_CHN);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VPSS_EnableChn failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
}

void VideoProcessImpl::StopVPSSDetectChn()
{
    int32_t ret;
    ret = HI_MPI_VPSS_DisableChn(NVR_VPSS_GRP, NVR_VPSS_DETECT_CHN);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_VPSS_DisableChn failed,code %#x", ret);
}

int32_t VideoProcessImpl::Initialize(const Params &params)
{
    if (init_)
        return static_cast<int>(KDupInitialize);
    err_code code;

    code = static_cast<err_code>(StartVPSSGroup(params));
    if (KSuccess != code)
        return static_cast<int>(code);

    code = static_cast<err_code>(StartVPSSEncodeChn(params));
    if (KSuccess != code)
        return static_cast<int>(code);

    code = static_cast<err_code>(StartVPSSDetectChn(params));
    if (KSuccess != code)
        return static_cast<int>(code);

    init_ = true;

    return static_cast<int>(KSuccess);
}

void VideoProcessImpl::Close()
{
    if (!init_)
        return;

    StopVPSSDetectChn();

    StopVPSSEncodeChn();

    StopVPSSGroup();

    init_ = false;
}

}; // namespace nvr
