#include "iplayer.hpp"

#include <assert.h>
#include <math.h>

#include "audio.hpp"


IPlayer::~IPlayer() {
    if (m_current_engine == NULL) return;

    ma_sound_uninit((ma_sound*)m_current_sound);
    ma_engine_uninit((ma_engine*)m_current_engine);
    free(m_current_engine);
}

bool
IPlayer::play_song(int index) {
    if (index < 0 || index >= m_songs.size()) return false;

    if (m_current_engine != NULL) {
        ma_sound_uninit((ma_sound*)m_current_sound);
        ma_engine_uninit((ma_engine*)m_current_engine);
        free(m_current_sound);
        free(m_current_engine);
        m_current_sound = NULL;
        m_current_engine = NULL;
    }

    std::list<ISongData>::iterator it = m_songs.begin();
    std::advance(it, index);
    ISongData song_to_play = *it;
    ma_engine* engine = (ma_engine*)malloc(sizeof(ma_engine));
    ma_sound* sound = (ma_sound*)malloc(sizeof(ma_sound));

    play_audio(song_to_play.get_song_file_path().c_str(), engine, sound);

    ma_sound_get_length_in_seconds(sound, &m_current_song_duration);
    ma_sound_get_length_in_pcm_frames(sound, &m_current_song_pcm_frames);

    m_current_engine = (void*)engine;
    m_current_sound = (void*)sound;
    m_current_song_idx = index;

    m_is_playing = true;
    return true;
}

bool
IPlayer::stop_song() {
    if (!m_is_playing || m_current_engine == nullptr) return false;

    stop_audio((ma_sound*)m_current_sound);

    m_is_playing = false;
    return true;
}

bool
IPlayer::continue_song() {
    if (m_is_playing) return false;

    continue_audio((ma_sound*)m_current_sound);

    m_is_playing = true;
    return true;
}

float
IPlayer::song_duration() {
    return m_current_sound != NULL
        ? m_current_song_duration
        : -1;
}

float
IPlayer::song_progressed_duration() {
    float result;
    ma_sound_get_cursor_in_seconds((ma_sound*)m_current_sound, &result);
    return result;
}

bool
IPlayer::is_song_ended() {
    return ma_sound_at_end((ma_sound*)m_current_sound) == 1;
}

bool
IPlayer::progress_to(int second) {
    float song_duration = m_current_song_duration;

    if (m_current_sound == NULL || second > song_duration) return false;

    float fsecond = (float)second;
    size_t pcm_frame_idx = round(
        (fsecond / song_duration) * m_current_song_pcm_frames);
    start_at((ma_sound*)m_current_sound, pcm_frame_idx);
    return true;
}

ISongData
IPlayer::current_song() {
    std::list<ISongData>::iterator it = m_songs.begin();
    std::advance(it, m_current_song_idx);
    return *it;
}

ISongData
IPlayer::add_song_to_list(std::string song_path) {
    if (song_path.empty()) return ISongData();

    ISongData new_song(m_songs.size(),
                       song_path);
    if (!new_song.is_valid_song()) return ISongData();

    m_songs.push_back(new_song);
    return new_song;
}
