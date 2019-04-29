#include "video_detect/video_detect_impl.h"
#include "common/res_code.h"
#include "base/ref_counted_object.h"

namespace nvr
{
rtc::scoped_refptr<VideoDetectModule> VideoDetectImpl::Create(const Params &params)
{
    err_code code;

    rtc::scoped_refptr<VideoDetectModule> implemention = new rtc::RefCountedObject<VideoDetectImpl>();

    code = static_cast<err_code>(implemention->Initialize(params));

    if (KSuccess != code)
    {
        log_e("error:%s", make_error_code(code).message().c_str());
        return nullptr;
    }

    return implemention;
}

int32_t VideoDetectImpl::StartMD(const Params &params)
{
    int32_t ret;

    // VDA_CHN_ATTR_S chn_attr;
    // chn_attr.enWorkMode = VDA_WORK_MODE_MD;
    // chn_attr.unAttr.stMdAttr.enVdaAlg = VDA_ALG_BG;
    // chn_attr.unAttr.stMdAttr.enMbSize = VDA_MB_16PIXEL;
    // chn_attr.unAttr.stMdAttr.enMbSadBits = VDA_MB_SAD_8BIT;
    // chn_attr.unAttr.stMdAttr.enRefMode = VDA_REF_MODE_DYNAMIC;
    // chn_attr.unAttr.stMdAttr.u32MdBufNum = 8;
    // chn_attr.unAttr.stMdAttr.u32VdaIntvl = params.frame_rate / 5;
    // chn_attr.unAttr.stMdAttr.u32BgUpSrcWgt = 128;
    // chn_attr.unAttr.stMdAttr.u32SadTh = 200;
    // chn_attr.unAttr.u32ObjNumMax = 64;
    // chn_attr.u32Width = params.width;
    // chn_attr.u32Height = params.height;

    // ret = HI_MPI_VDA_CreateChn(NVR_VDA_CHN, &chn_attr);
    // if (HI_SUCCESS != ret)
    // {
    //     log_e("HI_MPI_VDA_CreateChn failed,code %#x", ret);
    //     return static_cast<int>(KMPPError);
    // }

    // ret = HI_MPI_VDA_StartRecvPic(NVR_VDA_CHN);
    // if (HI_SUCCESS != ret)
    // {
    //     log_e("HI_MPI_VDA_StartRecvPic failed,code %#x", ret);
    //     return static_cast<int>(KMPPError);
    // }

    return static_cast<int>(KSuccess);
}

int32_t VideoDetectImpl::Initialize(const Params &params)
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    return static_cast<int>(KSuccess);
}
} // namespace nvr