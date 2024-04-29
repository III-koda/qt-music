#ifndef IPLAYER_H
#define IPLAYER_H

#include <list>
#include <map>
#include <string>

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

    float song_progressed_duration();

    bool is_song_ended();

    bool progress_to(int second);

    ISongData current_song();

    void clear_songs() { m_songs.clear(); }

    bool is_playing() { return m_is_playing; }

    int current_song_index() { return m_current_song_idx; }

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
