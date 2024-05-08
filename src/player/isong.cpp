#include "isong.hpp"

#include <math.h>
#include <string>

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>

#include "../utils/filesys.hpp"
#include "../utils/string.hpp"


ISongData::ISongData(int song_index,
                     std::string song_file_path) {
    m_song_index = song_index;
    m_song_file_path = song_file_path;
    m_isvalid = true;

    collect_metadata();
}

int
ISongData::get_song_index() {
    return m_song_index;
}

std::string
ISongData::get_song_file_path() {
    return m_song_file_path;
}

std::string
ISongData::get_duration() {
    return m_duration;
}

std::string
ISongData::get_play_type() {
    return m_play_type;
}

std::string
ISongData::get_song_title() {
    return m_song_title;
}

std::string
ISongData::get_artist() {
    return m_artist;
}

std::string
ISongData::generate_display_title() const {
    return m_artist.empty() || m_song_title.empty()
            ? get_file_name(m_song_file_path, false /* remove extension */)
            : replace_all(m_artist, "/", ", ") + " - " + m_song_title;
}

bool
ISongData::is_valid_song() {
    return m_isvalid;
}

void
ISongData::collect_metadata() {
    TagLib::FileRef f(m_song_file_path.c_str());

    if (f.isNull() || !f.audioProperties()) {
        m_isvalid = false;
        return;
    }

    TagLib::AudioProperties* aprop = f.audioProperties();

    unsigned int minutes = aprop->length() / 60;
    unsigned int rem_seconds = aprop->length() % 60;
    m_duration = std::to_string(minutes) + ":" + std::to_string(rem_seconds);

    if (f.tag()) {
        TagLib::Tag* tag = f.tag();

        m_song_title = std::string(tag->title().toCString(true));
        m_artist = std::string(tag->artist().toCString(true));
    }
}
