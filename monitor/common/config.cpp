#include "common/config.h"
#include "common/res_code.h"
#include "video_codec/video_codec_define.h"

#include <fstream>

#include <jsoncpp/json/json.h>

namespace nvr
{

int32_t Config::ReadConfigFile(const std::string &config_file)
{
    std::ifstream ifs(config_file, std::ios::binary);
    if (!ifs.is_open())
    {
        log_e("open config file %s failed", config_file.c_str());
        return static_cast<int>(KSystemError);
    }

    Json::Reader reader(Json::Features::strictMode());
    Json::Value root;

    if (!reader.parse(ifs, root))
    {
        log_e("parse config file failed,not json format");
        return static_cast<int>(KSystemError);
    }

    if (!root.isMember("video") ||
        !root["video"].isObject())
    {
        log_e("parse video config failed");
        return static_cast<int>(KSystemError);
    }

    Json::Value video = root["video"];
    if (!video.isMember("frame_rate") ||
        !video["frame_rate"].isInt() ||
        !video.isMember("width") ||
        !video["width"].isInt() ||
        !video.isMember("height") ||
        !video["height"].isInt() ||
        !video.isMember("codec_mode") ||
        !video["codec_mode"].isString() ||
        !video.isMember("codec_profile") ||
        !video["codec_profile"].isInt() ||
        !video.isMember("codec_bitrate") ||
        !video["codec_bitrate"].isInt())
    {
        log_e("parse video config failed");
        return static_cast<int>(KSystemError);
    }

    if (!root.isMember("detect") ||
        !root["detect"].isObject())
    {
        log_e("parse detect config failed");
        return static_cast<int>(KSystemError);
    }

    Json::Value detect = root["detect"];
    if (!detect.isMember("trigger_thresh") ||
        !detect["trigger_thresh"].isInt())
    {
        log_e("parse detect config failed");
        return static_cast<int>(KSystemError);
    }

    if (!root.isMember("record") ||
        !root["record"].isObject())
    {
        log_e("parse record config failed");
        return static_cast<int>(KSystemError);
    }

    Json::Value record = root["record"];
    if (!record.isMember("segment_duration") ||
        !record["segment_duration"].isInt() ||
        !record.isMember("path") ||
        !record["path"].isString() ||
        !record.isMember("use_md") ||
        !record["use_md"].isBool() ||
        !record.isMember("md_duration") ||
        !record["md_duration"].isInt())
    {
        log_e("parse record config failed");
        return static_cast<int>(KSystemError);
    }

    if (!root.isMember("rtmp") ||
        !root["rtmp"].isObject())
    {
        log_e("parse rtmp config failed");
        return static_cast<int>(KSystemError);
    }

    Json::Value rtmp = root["rtmp"];
    if (!rtmp.isMember("url") ||
        !rtmp["url"].isString())
    {
        log_e("parse rtmp config failed");
        return static_cast<int>(KSystemError);
    }

    //video
    this->video.frame_rate = video["frame_rate"].asInt();
    this->video.width = video["width"].asInt();
    this->video.height = video["height"].asInt();
    this->video.codec_mode = StringToCodecMode(video["codec_mode"].asString());
    this->video.codec_profile = video["codec_profile"].asInt();
    //detect
    this->detect.trigger_thresh = detect["trigger_thresh"].asInt();
    //record
    this->record.segment_duration =record["segment_duration"].asInt();
    this->record.path = record["path"].asString();
    this->record.use_md = record["use_md"].asBool();
    this->record.md_duration = record["md_duration"].asInt();
    //rtmp
    this->rtmp.url = rtmp["url"].asString();

    return static_cast<int>(KSuccess);
}

} // namespace nvr