#include "video_detect/video_detect_impl.h"
#include "common/res_code.h"
#include "common/system.h"
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

    ret = HI_IVS_MD_Init();
    if (HI_SUCCESS != ret)
    {
        log_e("HI_IVS_MD_Init failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }
    MD_ATTR_S attr;

    attr.enAlgMode = MD_ALG_MODE_BG;
    attr.enSadMode = IVE_SAD_MODE_MB_4X4;
    attr.enSadOutCtrl = IVE_SAD_OUT_CTRL_THRESH;
    attr.u16SadThr = 200;
    attr.u16Width = params.width;
    attr.u16Height = params.height;
    attr.stAddCtrl.u0q16X = 32768;
    attr.stAddCtrl.u0q16Y = 32768;
    attr.stCclCtrl.enMode = IVE_CCL_MODE_4C;
    attr.stCclCtrl.u16InitAreaThr = 16;
    attr.stCclCtrl.u16Step = 4;

    ret = HI_IVS_MD_CreateChn(NVR_MD_CHN, &attr);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_IVS_MD_CreateChn failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }
    return static_cast<int>(KSuccess);
}

void VideoDetectImpl::StopMD()
{
    int32_t ret;

    ret = HI_IVS_MD_DestroyChn(NVR_MD_CHN);
    if (HI_SUCCESS != ret && HI_ERR_IVE_NOTREADY != ret)
        log_e("HI_IVS_MD_DestroyChn failed,code %#x", ret);

    ret = HI_IVS_MD_Exit();
    if (HI_SUCCESS != ret)
        log_e("HI_IVS_MD_Exit failed,code %#x", ret);
}

int32_t VideoDetectImpl::IVEDMAImage(const VIDEO_FRAME_INFO_S &frame_info, const IVE_DST_IMAGE_S &dst_image, HI_BOOL instant)
{
    int32_t ret;

    IVE_HANDLE handle;
    IVE_SRC_DATA_S src_data;
    IVE_DST_DATA_S dst_data;
    IVE_DMA_CTRL_S dma_ctrl = {IVE_DMA_MODE_DIRECT_COPY, 0};
    HI_BOOL finish = HI_FALSE;
    HI_BOOL block = HI_TRUE;

    //fill src
    src_data.pu8VirAddr = (HI_U8 *)frame_info.stVFrame.pVirAddr[0];
    src_data.u32PhyAddr = frame_info.stVFrame.u32PhyAddr[0];
    src_data.u16Width = (HI_U16)frame_info.stVFrame.u32Width;
    src_data.u16Height = (HI_U16)frame_info.stVFrame.u32Height;
    src_data.u16Stride = (HI_U16)frame_info.stVFrame.u32Stride[0];

    //fill dst
    dst_data.pu8VirAddr = dst_image.pu8VirAddr[0];
    dst_data.u32PhyAddr = dst_image.u32PhyAddr[0];
    dst_data.u16Width = dst_image.u16Width;
    dst_data.u16Height = dst_image.u16Height;
    dst_data.u16Stride = dst_image.u16Stride[0];

    ret = HI_MPI_IVE_DMA(&handle, &src_data, &dst_data, &dma_ctrl, instant);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_IVE_DMA failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    if (HI_TRUE == instant)
    {
        ret = HI_MPI_IVE_Query(handle, &finish, block);
        while (HI_ERR_IVE_QUERY_TIMEOUT == ret)
        {
            usleep(100);
            ret = HI_MPI_IVE_Query(handle, &finish, block);
        }
        if (HI_SUCCESS != ret)
        {
            log_e("HI_MPI_IVE_Query failed,code %#x", ret);
            return static_cast<int>(KMPPError);
        }
    }

    return static_cast<int>(KSuccess);
}

int32_t VideoDetectImpl::AllocMemory(const Params &params)
{
    int32_t ret;
    for (int i = 0; i < 2; i++)
    {
        memset(&src_image_[i], 0, sizeof(src_image_[i]));
        src_image_[i].enType = IVE_IMAGE_TYPE_U8C1;
        src_image_[i].u16Width = params.width;
        src_image_[i].u16Height = params.height;
        src_image_[i].u16Stride[0] = System::Align(params.width, 16);
        ret = HI_MPI_SYS_MmzAlloc(&src_image_[i].u32PhyAddr[0], (void **)&src_image_[i].pu8VirAddr[0], NULL, HI_NULL, src_image_[i].u16Stride[0] * params.height);
        if (HI_SUCCESS != ret)
        {
            log_e("HI_MPI_SYS_MmzAlloc failed,code %#x,ret");
            return static_cast<int>(KMPPError);
        }
    }

    memset(&dst_mem_info_, 0, sizeof(dst_mem_info_));
    dst_mem_info_.u32Size = sizeof(IVE_CCBLOB_S);
    ret = HI_MPI_SYS_MmzAlloc(&dst_mem_info_.u32PhyAddr, (void **)&dst_mem_info_.pu8VirAddr, NULL, HI_NULL, dst_mem_info_.u32Size);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_SYS_MmzAlloc failed,code %#x,ret");
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
}

void VideoDetectImpl::FreeMemory()
{
    int32_t ret;
    ret = HI_MPI_SYS_MmzFree(dst_mem_info_.u32PhyAddr, dst_mem_info_.pu8VirAddr);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_SYS_MmzFree failed,code %#x", ret);

    for (int i = 0; i < 2; i++)
    {
        ret = HI_MPI_SYS_MmzFree(src_image_[i].u32PhyAddr[0], src_image_[i].pu8VirAddr[0]);
        if (HI_SUCCESS != ret)
            log_e("HI_MPI_SYS_MmzFree failed,code %#x", ret);
    }
}

void VideoDetectImpl::OnFrame(const VIDEO_FRAME_INFO_S &frame)
{
    if (!init_)
        return;

    int32_t ret;
    IVE_CCBLOB_S *ccbloc;

    if (first_frame_)
    {
        if (KSuccess != static_cast<err_code>(IVEDMAImage(frame, src_image_[index_], HI_TRUE)))
        {
            log_e("ive dma image failed");
            return;
        }
        index_ = 1 - index_;
        first_frame_ = false;
        return;
    }
    else
    {
        if (KSuccess != static_cast<err_code>(IVEDMAImage(frame, src_image_[index_], HI_TRUE)))
        {
            log_e("ive dma image failed");
            return;
        }
    }

    ret = HI_IVS_MD_Process(NVR_MD_CHN, &src_image_[index_], &src_image_[1 - index_], nullptr, &dst_mem_info_);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_IVS_MD_Process failed,code %#x", ret);
        return;
    }

    ccbloc = (IVE_CCBLOB_S *)(dst_mem_info_.pu8VirAddr);
    if (ccbloc->u8RegionNum > trigger_thresh_ && listener_)
        listener_->OnTrigger(ccbloc->u8RegionNum);

    index_ = 1 - index_;
}

int32_t VideoDetectImpl::Initialize(const Params &params)
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    err_code code;

    code = static_cast<err_code>(StartMD(params));
    if (KSuccess != code)
        return code;

    code = static_cast<err_code>(AllocMemory(params));
    if (KSuccess != code)
        return code;

    trigger_thresh_ = params.trigger_thresh;

    init_ = true;

    return static_cast<int>(KSuccess);
}

void VideoDetectImpl::Close()
{
    if (!init_)
        return;

    FreeMemory();

    StopMD();

    trigger_thresh_ = 0;
    listener_ = nullptr;
    first_frame_ = true;
    index_ = 0;
    init_ = false;
}

VideoDetectImpl::~VideoDetectImpl()
{
    Close();
}
VideoDetectImpl::VideoDetectImpl() : trigger_thresh_(0),
                                     listener_(nullptr),
                                     first_frame_(true),
                                     index_(0),
                                     init_(false)
{
}

void VideoDetectImpl::AddListener(DetectListener *listener)
{
    listener_ = listener;
}
} // namespace nvr