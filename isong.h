#ifndef ISONG_H
#define ISONG_H

#include <string>

class ISongData {
public:
    ISongData(int song_index,
              std::string song_file_path);

    int get_song_index();

    std::string get_song_file_path();

    std::string get_duration();

    std::string get_play_type();

    std::string get_song_title();

    std::string get_artist();

    bool is_valid_song();

private:
    void collect_metadata();

    int m_song_index;
    std::string m_song_file_path;
    std::string m_duration;
    std::string m_play_type;
    std::string m_song_title;
    std::string m_artist;
    bool m_isvalid;
};

#endif
