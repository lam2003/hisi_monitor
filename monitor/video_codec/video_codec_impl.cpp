#include "video_codec/video_codec_impl.h"
#include "common/res_code.h"
#include "common/system.h"

#include <base/ref_counted_object.h>

#define PACKET_BUFFER_SIZE (256 * 1024) //256kB

namespace nvr
{

rtc::scoped_refptr<VideoCodecModule> VideoCodecImpl::Create(const Params &params)
{
    err_code code;

    rtc::scoped_refptr<VideoCodecImpl> implemention = new rtc::RefCountedObject<VideoCodecImpl>();

    code = static_cast<err_code>(implemention->Initialize(params));

    if (KSuccess != code)
    {
        log_e("error:%s", make_error_code(code).message().c_str());
        return nullptr;
    }

    return implemention;
}

int32_t VideoCodecImpl::StartVENCChn(const Params &params)
{
    int32_t ret;

    VENC_CHN_ATTR_S chn_attr;
    memset(&chn_attr, 0, sizeof(chn_attr));

    chn_attr.stVeAttr.enType = PT_H264;

    VENC_ATTR_H264_S h264_attr;
    memset(&h264_attr, 0, sizeof(h264_attr));

    h264_attr.u32MaxPicWidth = PIC_WIDTH;
    h264_attr.u32MaxPicHeight = PIC_HEIGHT;
    h264_attr.u32PicWidth = params.width;
    h264_attr.u32PicHeight = params.height;
    h264_attr.u32BufSize = params.width * params.height * 2;
    h264_attr.u32Profile = params.profile;
    h264_attr.bByFrame = HI_FALSE;

    memcpy(&chn_attr.stVeAttr.stAttrH264e, &h264_attr, sizeof(h264_attr));
    if (params.codec_mode == CBR)
    {
        chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;

        VENC_ATTR_H264_CBR_S h264_cbr;
        memset(&h264_cbr, 0, sizeof(h264_cbr));

        h264_cbr.u32Gop = params.frame_rate;
        h264_cbr.u32StatTime = 1;
        h264_cbr.u32SrcFrmRate = params.frame_rate;
        h264_cbr.fr32DstFrmRate = params.frame_rate;
        h264_cbr.u32BitRate = params.bitrate;
        h264_cbr.u32FluctuateLevel = 1;

        memcpy(&chn_attr.stRcAttr.stAttrH264Cbr, &h264_cbr, sizeof(h264_cbr));
    }
    else if (params.codec_mode == VBR)
    {
        chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;

        VENC_ATTR_H264_VBR_S h264_vbr;
        memset(&h264_vbr, 0, sizeof(h264_vbr));

        h264_vbr.u32Gop = params.frame_rate;
        h264_vbr.u32StatTime = 1;
        h264_vbr.u32SrcFrmRate = params.frame_rate;
        h264_vbr.fr32DstFrmRate = params.frame_rate;
        h264_vbr.u32MinQp = 10;
        h264_vbr.u32MinIQp = 10;
        h264_vbr.u32MaxQp = 40;
        h264_vbr.u32MaxBitRate = params.bitrate;

        memcpy(&chn_attr.stRcAttr.stAttrH264Vbr, &h264_vbr, sizeof(h264_vbr));
    }
    else if (params.codec_mode == AVBR)
    {
        chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;

        VENC_ATTR_H264_AVBR_S h264_avbr;
        memset(&h264_avbr, 0, sizeof(h264_avbr));

        h264_avbr.u32Gop = params.frame_rate;
        h264_avbr.u32StatTime = 1;
        h264_avbr.u32SrcFrmRate = params.frame_rate;
        h264_avbr.fr32DstFrmRate = params.frame_rate;
        h264_avbr.u32MaxBitRate = params.bitrate;

        memcpy(&chn_attr.stRcAttr.stAttrH264AVbr, &h264_avbr, sizeof(h264_avbr));
    }
    else
    {
        log_e("unsupport codec mode:%d", static_cast<int>(params.codec_mode));
        return static_cast<int>(KMPPError);
    }

    chn_attr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
    chn_attr.stGopAttr.stNormalP.s32IPQpDelta = 0;

    ret = HI_MPI_VENC_CreateChn(NVR_VENC_CHN, &chn_attr);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VENC_CreateChn failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    ret = HI_MPI_VENC_StartRecvPic(NVR_VENC_CHN);
    if (HI_SUCCESS != ret)
    {
        log_e("HI_MPI_VENC_StartRecvPic failed,code %#x", ret);
        return static_cast<int>(KMPPError);
    }

    return static_cast<int>(KSuccess);
}

void VideoCodecImpl::StopVENCChn()
{
    int32_t ret;

    ret = HI_MPI_VENC_StopRecvPic(NVR_VENC_CHN);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_VENC_StopRecvPic failed,code %#x", ret);

    ret = HI_MPI_VENC_DestroyChn(NVR_VENC_CHN);
    if (HI_SUCCESS != ret)
        log_e("HI_MPI_VENC_DestroyChn failed,code %#x", ret);
}

void VideoCodecImpl::StartGetStreamThread(const Params &params)
{
    run_ = true;
    thread_ = std::unique_ptr<std::thread>(new std::thread([this, params]() {
        int32_t ret;

        VENC_STREAM_S stream;
        VENC_CHN_STAT_S chn_stat;

        prctl(PR_SET_NAME, "venc_thread", 0, 0, 0);

        int32_t fd = HI_MPI_VENC_GetFd(NVR_VENC_CHN);
        if (fd < 0)
        {
            log_e("HI_MPI_VENC_GetFd failed");
            return;
        }

        fd_set fds;
        timeval tv;

        void *packet_buf = malloc(PACKET_BUFFER_SIZE);
        uint32_t packet_buf_size = PACKET_BUFFER_SIZE;
        if (!packet_buf)
        {
            log_e("malloc packet buffer failed");
            return;
        }

        while (run_)
        {
            FD_ZERO(&fds);
            FD_SET(fd, &fds);

            tv.tv_sec = 1;
            tv.tv_usec = 0;

            ret = select(fd + 1, &fds, NULL, NULL, &tv);
            if (ret < 0)
            {
                log_e("select failed");
                return;
            }

            if (ret == 0)
                continue;

            if (FD_ISSET(fd, &fds))
            {
                memset(&stream, 0, sizeof(stream));
                memset(&chn_stat, 0, sizeof(chn_stat));
                ret = HI_MPI_VENC_Query(NVR_VENC_CHN, &chn_stat);
                if (HI_SUCCESS != ret)
                {
                    log_e("HI_MPI_VENC_Query failed,code %#x", ret);
                    return;
                }

                if (!chn_stat.u32CurPacks)
                {
                    log_w("current frame is null");
                    return;
                }

                if (sizeof(VENC_PACK_S) * chn_stat.u32CurPacks > packet_buf_size)
                {
                    free(packet_buf);
                    packet_buf = malloc(sizeof(VENC_PACK_S) * chn_stat.u32CurPacks);
                    if (!packet_buf)
                    {
                        log_e("malloc packet buffer failed");
                        return;
                    }
                    packet_buf_size = sizeof(VENC_PACK_S) * chn_stat.u32CurPacks;
                }
                stream.pstPack = (VENC_PACK_S *)packet_buf;
                stream.u32PackCount = chn_stat.u32CurPacks;

                ret = HI_MPI_VENC_GetStream(NVR_VENC_CHN, &stream, HI_TRUE);
                if (HI_SUCCESS != ret)
                {
                    log_e("HI_MPI_VENC_GetStream failed,code %#x", ret);
                    return;
                }

                for (uint32_t i = 0; i < stream.u32PackCount; i++)
                {

                    H264Frame frame;
                    frame.type = static_cast<int>(stream.pstPack[i].DataType.enH264EType);
                    frame.data = stream.pstPack[i].pu8Addr;
                    frame.len = stream.pstPack[i].u32Len;
                    frame.ts = stream.pstPack[i].u64PTS;
                    for (size_t j = 0; j < video_sinks_.size(); j++)
                        video_sinks_[j]->OnFrame(frame);
                }

                ret = HI_MPI_VENC_ReleaseStream(NVR_VENC_CHN, &stream);
                if (HI_SUCCESS != ret)
                {
                    log_e("HI_MPI_VENC_ReleaseStream failed,code %#x", ret);
                    return;
                }
            }
        }
        free(packet_buf);
    }));
}

void VideoCodecImpl::StopGetStreamThread()
{
    run_ = false;
    thread_->join();
    thread_.reset();
    thread_ = nullptr;
}

int32_t VideoCodecImpl::Initialize(const Params &params)
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    err_code code;

    code = static_cast<err_code>(StartVENCChn(params));
    if (KSuccess != code)
        return static_cast<int>(code);

    StartGetStreamThread(params);

    init_ = true;

    return static_cast<int>(KSuccess);
}
void VideoCodecImpl::AddVideoSink(VideoSinkInterface<VideoFrame> *video_sink)
{
    std::unique_lock<std::mutex> lock(mux_);
    video_sinks_.push_back(video_sink);
}

void VideoCodecImpl::ClearVideoSink()
{
    std::unique_lock<std::mutex> lock(mux_);
    video_sinks_.clear();
}

void VideoCodecImpl::Close()
{
    if (!init_)
        return;

    StopGetStreamThread();

    StopVENCChn();

    video_sinks_.clear();
    init_ = false;
}

VideoCodecImpl::VideoCodecImpl() : run_(false),
                                   thread_(nullptr),
                                   init_(false)
{
}

VideoCodecImpl::~VideoCodecImpl()
{
    Close();
}

}; // namespace nvr