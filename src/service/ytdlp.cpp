#include "ytdlp.hpp"

#include "../extlib/rapidjson/document.h"
#include "../extlib/rapidjson/writer.h"
#include "../extlib/rapidjson/stringbuffer.h"
#include "../extlib/logger.hpp"
#include "../utils/network.hpp"
#include "../utils/string.hpp"
#include "../utils/system.hpp"
#include "../utils/filesys.hpp"

#include <regex>

#define YTDLP_URL "https://github.com/yt-dlp/yt-dlp/releases/"

static std::string
get_latest_ytdlp_url() {
    std::string latest_url = std::string(YTDLP_URL) + "latest";
    HTTPResult res = make_http_request(HTTPMethod::GET,
                                       latest_url,
                                       NO_PARAM,
                                       true);

    if (res.successful && res.status == 200) {
        std::vector<std::string> tokens = split_string(res.location, '/');
        std::string version = tokens[tokens.size() - 1];

        Logger::get_instance()->log(LogLevel::INFO, "Downloading YTDLP version: " + version);
        std::string url = std::string(YTDLP_URL) + "download/" + version + "/yt-dlp";
        if (is_mac()) {
            url += "_macos";
        } else if (is_linux()) {
            url += "_linux";
        } else {
            return "";
        }
        return url;
    }
    Logger::get_instance()->log(LogLevel::ERROR, "failed to obtain latest YTDLP url: " + res.location);
    return "";
}

bool download_ytdlp() {
    std::string ytdlp_path = app_directory() + "/yt-dlp";

    if (exists(ytdlp_path)){
        return true;
    } else {
        std::string ytdlp_url = get_latest_ytdlp_url();
        if (ytdlp_url.empty()) {
            return false;
        }
        if (download_file(ytdlp_url, ytdlp_path)) {
            std::string cmd = std::string("/bin/chmod +x ") + ytdlp_path;
            int retval = std::system(cmd.c_str());
            return WIFEXITED(retval);
        } else {
            Logger::get_instance()->log(LogLevel::ERROR, "cannot download YTDLP");
            return false;
        }
    }
}


/**
 * @brief Extract video ID from a Youtube URL. Something must be wrong if an
 *        empty string is returned.
 */
static std::string
extract_video_id(const std::string& url) {
    std::regex regex("(?:https?:\\/\\/)?(?:www\\.)?(?:youtube\\.com\\/watch\\?v=|youtu\\.be\\/)([a-zA-Z0-9_-]+)");
    std::smatch match;
    return std::regex_search(url, match, regex)
            ? match[1].str()
            : "";
}


/**
 * @brief Get a Youtube video's data from the video ID.
 */
static rapidjson::Document
get_video_info(const std::string& video_id) {
    std::map<std::string, std::string> params {
        {"part", "snippet"},
        {"key", "AIzaSyBX5a7LHxeOvFaVXbGrFICQWTH5MIJKzGw"},
        {"id", video_id}
    };
    HTTPResult res = make_http_request(HTTPMethod::GET,
                                       "https://www.googleapis.com/youtube/v3/videos",
                                       params);
    rapidjson::Document d;
    if (!res.successful || res.status != 200) {
        return d;
    }
    d.Parse(res.body.c_str());
    return d;
}


/**
 * @brief Get a Youtube channel's data from the channel ID.
 */
static rapidjson::Document
get_channel_info(const std::string& channel_id) {
    std::map<std::string, std::string> params {
        {"part", "snippet"},
        {"key", "AIzaSyBX5a7LHxeOvFaVXbGrFICQWTH5MIJKzGw"},
        {"id", channel_id}
    };
    HTTPResult res = make_http_request(HTTPMethod::GET,
                                       "https://www.googleapis.com/youtube/v3/channels",
                                       params);
    rapidjson::Document d;
    if (!res.successful || res.status != 200) {
        return d;
    }
    d.Parse(res.body.c_str());
    return d;
}


/**
 * @brief Download cover art for a Youtube video from the given video JSON data.
 *
 * @param video_json    The JSON data of the video.
 * @param save_as       Name of the image to be saved as.
 *
 * @return True if successfully download the cover art, false otherise.
 */
bool
get_cover_art_from_video_data(const rapidjson::Document& video_json,
                              const std::string& save_as) {
    if (!video_json.IsObject() ||
            !video_json.HasMember("items") ||
            !video_json["items"].IsArray() ||
            video_json["items"].Empty() ||
            !video_json["items"][0].IsObject() ||
            !video_json["items"][0].HasMember("snippet") ||
            !video_json["items"][0]["snippet"].IsObject() ||
            !video_json["items"][0]["snippet"].HasMember("channelId") ||
            !video_json["items"][0]["snippet"]["channelId"].IsString()) {
        return false;
    }
    std::string channel_id = video_json["items"][0]["snippet"]["channelId"].GetString();
    rapidjson::Document channeljson = get_channel_info(channel_id);
    if (!channeljson.IsObject() ||
            !channeljson.HasMember("items") ||
            !channeljson["items"].IsArray() ||
            channeljson["items"].Empty() ||
            !channeljson["items"][0].IsObject() ||
            !channeljson["items"][0].HasMember("snippet") ||
            !channeljson["items"][0]["snippet"].IsObject() ||
            !channeljson["items"][0]["snippet"].HasMember("thumbnails") ||
            !channeljson["items"][0]["snippet"]["thumbnails"].IsObject()) {
        return false;
    }
    auto thumbnails = channeljson["items"][0]["snippet"]["thumbnails"].GetObject();
    // Get whatever image available, but prioritize the best quality one.
    std::string img_url;
    if (thumbnails.HasMember("default") &&
            thumbnails["default"].IsObject() &&
            thumbnails["default"].HasMember("url") &&
            thumbnails["default"]["url"].IsString()) {
        img_url = thumbnails["default"]["url"].GetString();
    }
    if (thumbnails.HasMember("medium") &&
            thumbnails["medium"].IsObject() &&
            thumbnails["medium"].HasMember("url") &&
            thumbnails["medium"]["url"].IsString()) {
        img_url = thumbnails["medium"]["url"].GetString();
    }
    if (thumbnails.HasMember("high") &&
            thumbnails["high"].IsObject() &&
            thumbnails["high"].HasMember("url") &&
            thumbnails["high"]["url"].IsString()) {
        img_url = thumbnails["high"]["url"].GetString();
    }
    return img_url.empty()
            ? false
            : download_file(img_url, save_as);
}


bool download_youtube_audio(std::string url, std::string save_dir) {
    if (!starts_with(url, "https://www.youtube.com/watch")) {
        return false;
    }
    std::string vid_id = extract_video_id(url);
    if (vid_id.empty()) {
        return false;
    }
    std::string download_as = save_dir + "/" + vid_id + ".mp3";

    std::string command = app_directory() + "/yt-dlp " +
                          "--extract-audio --audio-format mp3 " +
                          "-o " + download_as + " " +
                          "\"" + url + "\"";
    CommandResult result;
    try {
        result = run_cmd(command);
    } catch (...) {
        return false;
    }

    if (result.exit_status() != 0) {
        return false;
    }

    // Data about the video
    rapidjson::Document video_json = get_video_info(vid_id);

    if (!video_json.IsObject() ||
            !video_json.HasMember("items") ||
            !video_json["items"].IsArray() ||
            video_json["items"].Empty() ||
            !video_json["items"][0].IsObject() ||
            !video_json["items"][0].HasMember("snippet") ||
            !video_json["items"][0]["snippet"].IsObject() ||
            !video_json["items"][0]["snippet"].HasMember("title") ||
            !video_json["items"][0]["snippet"]["title"].IsString()) {
        return true; // Return true since the audio was still downloaded anyway.
    }

    std::string title = video_json["items"][0]["snippet"]["title"].GetString();
    // The name that this video will be renamed to afterwards.
    std::string rename_to = save_dir + "/" + title + ".mp3";
    // If rename_to exists, it means the same video was already downloaded to
    // this directory.
    if (exists(rename_to)) {
        remove_file(download_as);
        return false;
    }

    std::string cover_art_filepath = save_dir + "/" + vid_id + ".jpg";
    if (get_cover_art_from_video_data(video_json, cover_art_filepath)) {
        (void)embed_image_to_audio_file(download_as, cover_art_filepath);
        remove_file(cover_art_filepath);
    }

    rename_file(download_as, rename_to);
    return true;
}
