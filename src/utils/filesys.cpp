#include "filesys.hpp"

#ifndef __has_include
static_assert(false, "__has_include not supported");
#else
#if __cplusplus >= 201703L && __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#elif __has_include(<boost/filesystem.hpp>)
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#endif
#endif

#include <fstream>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <taglib/taglib.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>

#include "network.hpp"
#include "string.hpp"


std::string app_directory() {
    std::string homedir;

    char* home = getenv("HOME");
    if (home == NULL) {
        char* user = getenv("USER");
        if (user == NULL) {
            return ".qt-music";
        } else {
            homedir += "/home/" + std::string(user);
        }
    } else {
        homedir = home;
    }
    return std::string(homedir) + "/.qt-music";
}

void create_directory(std::string path) {
    if (!fs::exists(path)) {
        fs::create_directory(path);
    }
}

bool exists(std::string path) {
    return fs::exists(path);
}

void rename_file(const std::string& oldname, const std::string& newname) {
    if (!exists(oldname)) {
        return;
    }
    fs::rename(oldname, newname);
}

void remove_file(const std::string& filename) {
    fs::remove(filename);
}


std::vector<std::string> files_in_dir(const std::string &dir_path)
{
    std::vector<std::string> result;

    for (const auto &entry : fs::directory_iterator(dir_path))
    {
        if (entry.is_directory())
            continue;

        std::string filepath = entry.path().string();
        replace_all_in_place(filepath, "\\", "/");

        result.push_back(filepath);
    }
    return result;
}


bool
download_file(const std::string &file_url, const std::string &save_as)
{
    HTTPResult res = make_http_request(HTTPMethod::GET, file_url, NO_PARAM, true);
    if (!res.successful || res.status != 200) {
        return false;
    }
    std::string path = save_as.c_str();

    std::ofstream file(path, std::ios::binary);
    if (file.is_open()) {
        file.write(res.body.data(), res.body.size());
    }
    else {
        return false;
    }

    return true;
}


std::string
get_file_name(const std::string& filepath, bool include_extension) {
    std::vector<std::string> tokens = split_string(filepath, '/');
    if (tokens.empty()) {
        return "";
    }
    if (include_extension) {
        return tokens[tokens.size() - 1];
    }
    size_t lastindex = tokens[tokens.size() - 1].find_last_of(".");
    return tokens[tokens.size() - 1].substr(0, lastindex);
}


bool
create_file_if_not_exists(const std::string& file_name) {
    std::ifstream in_file(file_name);
    if (in_file.good()) {
        return true; // File already exists
    } else {
        // File does not exist, create it
        std::ofstream out_file(file_name);
        if (out_file) {
            return true;
        } else {
            return false;
        }
    }
}


class ImageFile : public TagLib::File {
public:
    ImageFile(const char *file) : TagLib::File(file) {}

    TagLib::ByteVector data() {
        return readBlock(length());
    }

private:
    virtual TagLib::Tag *tag() const { return 0; }
    virtual TagLib::AudioProperties *audioProperties() const { return 0; }
    virtual bool save() { return false; }
};


bool embed_image_to_audio_file(const std::string& audio_file_path,
                               const std::string& cover_art_path) {
    std::vector<std::string> tokens = split_string(audio_file_path, '.');
    std::string filetype = tokens.empty()
            ? std::string()
            : upper(tokens[tokens.size() - 1]);

    ImageFile image_file(cover_art_path.c_str());

    if (filetype == "MP3") {
        TagLib::MPEG::File audio_file(audio_file_path.c_str());
        TagLib::ID3v2::Tag* tag = audio_file.ID3v2Tag(true);
        TagLib::ID3v2::AttachedPictureFrame* frame = new TagLib::ID3v2::AttachedPictureFrame;

        frame->setMimeType("image/jpeg");
        frame->setPicture(image_file.data());

        tag->addFrame(frame);
        audio_file.save();
    } else {
        // Unsupported audio type.
        return false;
    }
    return true;
}
