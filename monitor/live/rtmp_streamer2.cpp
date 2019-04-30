#include "live/rtmp_streamer2.h"
#include "common/res_code.h"

#define RTMP_STREAMER_BUFFER_SIZE 131072 //128k
#define RTMP_HEADER_SIZE (sizeof(RTMPPacket) + RTMP_MAX_HEADER_SIZE)

namespace nvr
{

RTMPStreamer2::RTMPStreamer2() : sps_(""),
                                 pps_(""),
                                 send_meta_(false),
                                 rtmp_(nullptr),
                                 buf_(nullptr),
                                 init_(false)
{
}

RTMPStreamer2::~RTMPStreamer2()
{
    Close();
}

void RTMPStreamer2::Close()
{
    if (!init_)
        return;

    RTMP_Close(rtmp_);
    RTMP_Free(rtmp_);
    free(buf_);

    sps_ = "";
    pps_ = "";
    send_meta_ = false;
    rtmp_ = nullptr;
    buf_ = nullptr;

    init_ = false;
}

int32_t RTMPStreamer2::Initialize(const std::string &url)
{
    if (init_)
        return static_cast<int>(KDupInitialize);

    buf_ = (uint8_t *)malloc(RTMP_STREAMER_BUFFER_SIZE);
    if (!buf_)
    {
        log_e("malloc rtmp streamer buffer failed");
        return static_cast<int>(KSystemError);
    }
    //rtmpdump初始化函数不可重入,需要加锁保证线程同步
    static std::mutex KMux;

    KMux.lock();
    rtmp_ = RTMP_Alloc();
    if (!rtmp_)
    {
        KMux.unlock();
        log_e("RTMP_Alloc failed");
        return static_cast<int>(KThirdPartyError);
    }
    RTMP_Init(rtmp_);
    KMux.unlock();

    if (RTMP_SetupURL(rtmp_, const_cast<char *>(url.c_str())) == false)
    {
        log_e("RTMP_SetupURL failed");
        return static_cast<int>(KThirdPartyError);
    }

    RTMP_EnableWrite(rtmp_);

    if (!RTMP_Connect(rtmp_, NULL))
    {
        log_e("RTMP_EnableWrite failed");
        return static_cast<int>(KThirdPartyError);
    }

    if (!RTMP_ConnectStream(rtmp_, 0))
    {
        log_e("RTMP_ConnectStream failed");
        return static_cast<int>(KThirdPartyError);
    }
    init_ = true;

    return static_cast<int>(KSuccess);
}

int32_t RTMPStreamer2::SendH264MetaData(const std::string &sps, const std::string &pps)
{
    int32_t ret;
    RTMPPacket *pkt;

    pkt = (RTMPPacket *)(buf_);
    pkt->m_body = (char *)(pkt + RTMP_HEADER_SIZE);

    uint32_t pos = 0;
    uint8_t *body = (uint8_t *)pkt->m_body;

    body[pos++] = 0x17;
    body[pos++] = 0x00;

    body[pos++] = 0x00;
    body[pos++] = 0x00;
    body[pos++] = 0x00;

    body[pos++] = 0x01;
    body[pos++] = sps.c_str()[5];
    body[pos++] = sps.c_str()[6];
    body[pos++] = sps.c_str()[7];
    body[pos++] = 0xff;

    body[pos++] = 0xe1;
    body[pos++] = ((sps.length() - 4) >> 8) & 0xff;
    body[pos++] = (sps.length() - 4) & 0xff;
    memcpy(&body[pos], sps.c_str() + 4, sps.length() - 4);
    pos += (sps.length() - 4);

    body[pos++] = 0x01;
    body[pos++] = ((pps.length() - 4) >> 8) & 0xff;
    body[pos++] = (pps.length() - 4) & 0xff;
    memcpy(&body[pos], pps.c_str() + 4, pps.length() - 4);
    pos += (pps.length() - 4);

    pkt->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    pkt->m_nBodySize = pos;
    pkt->m_nChannel = 0x04;
    pkt->m_nTimeStamp = 0;
    pkt->m_hasAbsTimestamp = 0;
    pkt->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
    pkt->m_nInfoField2 = rtmp_->m_stream_id;

    ret = RTMP_SendPacket(rtmp_, pkt, true);
    if (!ret)
    {
        log_e("RTMP_SendPacket failed,code %d", ret);
        return static_cast<int>(KThirdPartyError);
    }

    return static_cast<int>(KSuccess);
}

int32_t RTMPStreamer2::WriteVideoFrame(const VideoFrame &frame)
{
    if (frame.type == H264Frame::NaluType::SPS)
    {
        sps_ = std::string((char *)frame.data, frame.len);
        return static_cast<int>(KSuccess);
    }
    else if (frame.type == H264Frame::NaluType::PPS)
    {
        pps_ = std::string((char *)frame.data, frame.len);
        return static_cast<int>(KSuccess);
    }
    else if (frame.type == H264Frame::NaluType::SEI)
    {
        return static_cast<int>(KSuccess);
    }

    if (sps_ == "" || pps_ == "")
        return static_cast<int>(KSuccess);

    if (!send_meta_)
    {
        err_code code = static_cast<err_code>(SendH264MetaData(sps_, pps_));
        if (KSuccess != code)
            return static_cast<int>(code);
        send_meta_ = true;
    }

    int32_t ret;
    RTMPPacket *pkt;

    pkt = (RTMPPacket *)buf_;
    pkt->m_body = (char *)(pkt + RTMP_HEADER_SIZE);

    uint32_t pos = 0;
    uint8_t *body = (uint8_t *)pkt->m_body;

    body[pos++] = (frame.type == H264Frame::NaluType::ISLICE ? 0x17 : 0x27);
    body[pos++] = 0x01;
    body[pos++] = 0x00;
    body[pos++] = 0x00;
    body[pos++] = 0x00;

    body[pos++] = (frame.len - 4) >> 24 & 0xff;
    body[pos++] = (frame.len - 4) >> 16 & 0xff;
    body[pos++] = (frame.len - 4) >> 8 & 0xff;
    body[pos++] = (frame.len - 4) & 0xff;

    memcpy(&body[pos], frame.data + 4, frame.len - 4);

    pos += (frame.len - 4);

    pkt->m_nBodySize = pos;
    pkt->m_hasAbsTimestamp = 0;
    pkt->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    pkt->m_nInfoField2 = rtmp_->m_stream_id;
    pkt->m_nChannel = 0x04;
    pkt->m_nTimeStamp = RTMP_GetTime();
    pkt->m_headerType = RTMP_PACKET_SIZE_LARGE;

    ret = RTMP_SendPacket(rtmp_, pkt, true);
    if (!ret)
    {
        log_e("RTMP_SendPacket failed,code %d", ret);
        return static_cast<int>(KThirdPartyError);
    }

    return static_cast<int>(KSuccess);
}

} // namespace nvr
