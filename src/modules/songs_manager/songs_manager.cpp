#include "songs_manager.hpp"

#include <algorithm>
#include <assert.h>
#include <math.h>
#include <stdexcept>

#include <taglib/fileref.h>

#include "audio.hpp"


SongsManager::~SongsManager() {
    // Set m_running to false and sleep so the loop in `run` has time to
    // completely stop.
    m_running = false;
    usleep(500000 /* 500000 microsec = 0.5 sec */);

    if (m_current_engine == NULL) {
        return;
    }
    ma_sound_uninit((ma_sound*)m_current_sound);
    ma_engine_uninit((ma_engine*)m_current_engine);
    free(m_current_engine);
}

bool
SongsManager::play_song(int index) {
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
SongsManager::stop_song() {
    if (!m_is_playing || m_current_engine == nullptr) return false;

    stop_audio((ma_sound*)m_current_sound);

    m_is_playing = false;
    return true;
}

bool
SongsManager::continue_song() {
    if (m_is_playing) return false;

    continue_audio((ma_sound*)m_current_sound);

    m_is_playing = true;
    return true;
}

float
SongsManager::song_duration() {
    return m_current_sound != NULL
        ? m_current_song_duration
        : -1;
}

float
SongsManager::song_progressed_duration() const {
    float result;
    ma_sound_get_cursor_in_seconds((ma_sound*)m_current_sound, &result);
    return result;
}

bool
SongsManager::is_song_ended() const {
    return ma_sound_at_end((ma_sound*)m_current_sound) == 1;
}

bool
SongsManager::progress_to(int second) {
    float song_duration = m_current_song_duration;

    if (m_current_sound == NULL || second > song_duration) return false;

    float fsecond = (float)second;
    size_t pcm_frame_idx = round(
        (fsecond / song_duration) * m_current_song_pcm_frames);
    start_at((ma_sound*)m_current_sound, pcm_frame_idx);
    return true;
}

ISongData
SongsManager::get_song_at_index(int index) const {
    if (index < 0 || index >= m_songs.size()) {
        throw std::invalid_argument( "Index out of range" );
    }
    std::list<ISongData>::const_iterator it = m_songs.begin();
    std::advance(it, index);
    return *it;
}

ISongData
SongsManager::add_song_to_list(std::string song_path) {
    if (song_path.empty()) return ISongData();

    ISongData new_song(m_songs.size(),
                       song_path);
    if (!new_song.is_valid_song()) return ISongData();

    m_songs.push_back(new_song);
    return new_song;
}

bool
SongsManager::is_audio_file_valid(std::string filepath) const {
    TagLib::FileRef f(filepath.c_str());
    return !f.isNull() && f.audioProperties();
}


void
SongsManager::subscribe_for_song_changes(SongListener* listener) {
    for (SongListener* existing_listener : m_listeners) {
        if (listener == existing_listener) {
            return;
        }
    }
    m_listeners.push_back(listener);
}

void
SongsManager::unsubscribe_from_song_changes(SongListener* listener) {
    m_listeners.erase(
            std::remove(m_listeners.begin(), m_listeners.end(), listener),
            m_listeners.end());
}

void
SongsManager::run() {
    while (m_running) {
        if (!m_is_playing) {
            usleep(500000 /* 500000 microsec = 0.5 sec */);
            continue;
        }

        float curr_progress = song_progressed_duration();
        if (curr_progress != m_song_last_progress) {
            for (SongListener* listener : m_listeners) {
                listener->song_progressed_duration_changed(song_progressed_duration());
            }
            m_song_last_progress = curr_progress;
        }
        if (is_song_ended()) {
            for (SongListener* listener : m_listeners) {
                listener->current_song_ended();
            }
        }
        usleep(500000 /* 500000 microsec = 0.5 sec */);
    }
}
