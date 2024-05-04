#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "cover_art_api.hpp"

#include "../extlib/rapidjson/document.h"
#include "../extlib/rapidjson/writer.h"
#include "../extlib/rapidjson/stringbuffer.h"
#include "../extlib/httplib.h"
#include "../utils/utils.hpp"

#include <sys/stat.h>
#include <taglib/taglib.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/attachedpictureframe.h>

httplib::Headers BASIC_HTTP_HEADERS = {
    {"User-Agent", "QtMusicPlayer/1.0.0"},
    {"Accept", "application/json"}
};

static std::string
generate_image_file_path(ISongData song_data) {
    std::string DIR = "/tmp/qt-music";
    struct stat sb;
    if (stat(DIR.c_str(), &sb) != 0) {
        (void)mkdir(DIR.c_str(), 0777);
    }
    return DIR + "/cover_art.jpg";
}

static rapidjson::Document
get_recording_data(ISongData song_data) {
    rapidjson::Document d;
    httplib::Client cli("https://musicbrainz.org");
    cli.enable_server_certificate_verification(false);

    std::string params = std::string("?")
            + "limit=1&"
            + "query=%22"
            + song_data.get_song_title()
            + "%20AND%20"
            + "artist%3A%22"
            + song_data.get_artist()
            + "%22";
    replace_all_in_place(params, " ", "%20");
    httplib::Result res = cli.Get("/ws/2/recording" + params, BASIC_HTTP_HEADERS);

    if (!res) {
        return d;
    }
    d.Parse(res->body.c_str());
    return d;
}

static rapidjson::Document
get_release_data(std::string release_id) {
    rapidjson::Document d;

    httplib::Client cli("https://coverartarchive.org");
    cli.enable_server_certificate_verification(false);
    httplib::Result res = cli.Get("/release/" + release_id, BASIC_HTTP_HEADERS);
    if (!res) {
        return d;
    }
    std::string body = trim(res->body);
    if (!starts_with(body, "See:")) {
        return d;
    }
    std::vector<std::string> tokens = split_string(body, ' ');
    if (tokens.size() < 2) {
        return d;
    }

    std::string json_file_url = tokens[1];
    std::string base_url = get_base_url(json_file_url);
    httplib::Client cli2(base_url);
    cli2.enable_server_certificate_verification(false);
    cli2.set_follow_location(true);
    res = cli2.Get(replace_all(json_file_url, base_url, ""), BASIC_HTTP_HEADERS);
    if (!res) {
        return d;
    }
    d.Parse(res->body.c_str());
    return d;
}

static std::string
get_cover_art_from_music_brainz(ISongData song_data)
{
    if (song_data.get_artist().empty() || song_data.get_song_title().empty()) {
        return "";
    }

    rapidjson::Document recording_json = get_recording_data(song_data);
    if (!recording_json.IsObject()) {
        return "";
    }
    std::string release_id = recording_json["recordings"][0]["releases"][0]["id"].GetString();

    rapidjson::Document release_json = get_release_data(release_id);
    if (!release_json.IsObject()) {
        return "";
    }

    std::string image_url = release_json["images"][0]["image"].GetString();

    std::string img_file = generate_image_file_path(song_data);
    if (!download_file(image_url, img_file)) {
        return "";
    }

    return img_file;
}

static std::string
get_cover_art_from_metadata(ISongData song_data) {
    TagLib::MPEG::File file(song_data.get_song_file_path().c_str());

    // Access the ID3v2 tag
    TagLib::ID3v2::Tag *tag = file.ID3v2Tag();

    // Check if the tag exists
    if (tag) {
        // Retrieve the list of attached picture frames
        TagLib::ID3v2::FrameList frames = tag->frameListMap()["APIC"];

        // Iterate through each attached picture frame
        for (TagLib::ID3v2::Frame *frame : frames) {
            TagLib::ID3v2::AttachedPictureFrame *pictureFrame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(frame);
            if (pictureFrame) {
                // Retrieve the MIME type and picture data
                std::string mimeType = pictureFrame->mimeType().to8Bit(true);
                TagLib::ByteVector imageData = pictureFrame->picture();

                std::string img_path = generate_image_file_path(song_data);
                FILE *imageFile = fopen(img_path.c_str(), "wb");
                fwrite(imageData.data(), 1, imageData.size(), imageFile);
                fclose(imageFile);
                return img_path;
            }
        }
    }
    return "";
}

std::string
get_cover_art(ISongData song_data) {
    std::string img_path = get_cover_art_from_metadata(song_data);
    if (!img_path.empty()) {
        return img_path;
    }
    return get_cover_art_from_music_brainz(song_data);
}
