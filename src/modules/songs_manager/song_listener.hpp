#ifndef SONGS_LISTENER_HPP
#define SONGS_LISTENER_HPP

class SongListener {
public:
    virtual void current_song_ended() = 0;

    virtual void song_progressed_duration_changed(float new_progress_duration) = 0;
};

#endif // SONG_LISTENER_HPP
