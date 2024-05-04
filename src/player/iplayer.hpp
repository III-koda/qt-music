#ifndef IPLAYER_H
#define IPLAYER_H

#include <list>
#include <map>
#include <string>

#include <taglib/fileref.h>

#include "isong.hpp"


class IPlayer {
public:
    IPlayer() : m_is_playing(false),
                m_current_song_idx(-1),
                m_current_engine(NULL),
                m_current_sound(NULL) {}

    ~IPlayer();

    bool play_song(int index);

    bool stop_song();

    bool continue_song();

    ISongData add_song_to_list(std::string songpath);

    float song_duration();

    float song_progressed_duration() const;

    bool is_song_ended() const;

    bool progress_to(int second);

    ISongData get_song_at_index(int index) const;

    bool is_audio_file_valid(std::string filepath) const;

    ISongData current_song() const{
        return get_song_at_index(m_current_song_idx);
    }

    void clear_songs() {
        m_songs.clear();
    }

    bool is_playing() const {
        return m_is_playing;
    }

    int current_song_index() const {
        return m_current_song_idx;
    }

    void set_current_song_index(int idx) {
        m_current_song_idx = idx;
    }

    size_t songs_count() const {
        return m_songs.size();
    }

private:
    bool m_is_playing;
    int m_current_song_idx;
    float m_current_song_duration;
    unsigned long long m_current_song_pcm_frames;
    void* m_current_engine;
    void* m_current_sound;
    std::list<ISongData> m_songs;
};

#endif // IPLAYER_H
