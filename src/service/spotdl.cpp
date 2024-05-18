#include "spotdl.hpp"

#include "../utils/filesys.hpp"
#include "../utils/network.hpp"
#include "../utils/string.hpp"
#include "../extlib/logger.hpp"

#include <filesystem>

#include <QString>

#define SPOTDL_URL "https://github.com/spotDL/spotify-downloader/releases"

static std::string
get_latest_spotdl_version_url() {
    std::string latest_url = std::string(SPOTDL_URL) + "/latest";
    HTTPResult res = make_http_request(HTTPMethod::GET,
                                       latest_url,
                                       NO_PARAM,
                                       true);

    if (res.successful && res.status == 200) {
        std::vector<std::string> tokens = split_string(res.location, '/');
        std::string version = tokens[tokens.size() - 1];
        replace_all_in_place(version, "v", "");
        Logger::get_instance()->log(LogLevel::INFO, "Downloading spotdl version: v" + version);
        return std::string(SPOTDL_URL) + "/download/v" + version + "/spotdl-" + version + "-linux";
    }
    Logger::get_instance()->log(LogLevel::ERROR, "failed to obtain latest SpotDL url: " + res.location);
    return "";
}

bool
download_spotdl() {
    std::string spotdl_path = app_directory() + "/spotdl";

    if (exists(spotdl_path)){
        return true;
    } else {
        std::string spotdl_url = get_latest_spotdl_version_url();
        if (spotdl_url.empty()){
            return false;
        }
        if (download_file(spotdl_url, spotdl_path)) {
            std::string cmd = std::string("/bin/chmod +x ") + spotdl_path;
            return WIFEXITED(std::system(cmd.c_str()));
        } else {
            Logger::get_instance()->log(LogLevel::ERROR, "cannot download SpotDL");
            return false;
        }
    }
}

bool
download_spotify_song(const std::string spotify_url, std::string save_dir) {
    if (!download_spotdl()) {
        return false;
    }
    if (spotify_url.empty()) {
        return false;
    }

    std::string command = std::string("cd ") + save_dir + " && " +
                          app_directory() + "/spotdl " + spotify_url;

    int result = std::system(command.c_str());
    if (result != 0) {
        return false;
    }
    return true;
}
