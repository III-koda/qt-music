#include "spotdl.hpp"

#include "../extlib/httplib.h"
#include "../utils/utils.hpp"

#include <filesystem>

#include <QString>
#include <QDebug>

#define SPOTDL_EXE_URL "https://github.com/spotDL/spotify-downloader/releases/download/v4.2.5/spotdl-4.2.5-linux"
#define QTMUSIC_DIR "/home/bao/.qt-music"


bool
download_spotdl() {
    std::filesystem::path spotdl_path = "/home/bao/.qt-music/spotdl";

    if (std::filesystem::exists(spotdl_path)){
        return true;
    } else {
        std::filesystem::create_directory(QTMUSIC_DIR);
        if (download_file(SPOTDL_EXE_URL, spotdl_path)) {
            std::string cmd = std::string("/bin/chmod +x ") + QTMUSIC_DIR + "/spotdl";
            return WIFEXITED(std::system(cmd.c_str()));
        } else {
            return false;
        }
    }
}


bool download_spotify_song(const std::string spotify_url, std::string save_dir) {
    if (!download_spotdl()) {
        qDebug() << QString("Error: spotdl executable not found.");
        return false;
    }
    if (spotify_url.empty()) {
        return false;
    }

    std::string command = std::string("cd ") + save_dir + + " && " +
                          QTMUSIC_DIR + "/spotdl " + spotify_url;

    int result = std::system(command.c_str());
    
    if (result != 0) {
        qDebug() << QString("Error: Failed to download song.");
        return false;
    }

    qDebug() << QString("Song downloaded successfully.");
    return true;
}
