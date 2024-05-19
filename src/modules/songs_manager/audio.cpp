#define MINIAUDIO_IMPLEMENTATION

#include "audio.hpp"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

bool play_audio(const char* audio_file_path,
                ma_engine* engine,
                ma_sound* sound) {
    if (ma_engine_init(NULL, engine) != MA_SUCCESS ||
        ma_sound_init_from_file(engine, audio_file_path,
                                0, NULL, NULL, sound) != MA_SUCCESS) {
        return false;
    }

    ma_sound_start(sound);
    return true;
}

bool stop_audio(ma_sound* sound) {
    return sound == NULL
        ? false
        : ma_sound_stop(sound) == MA_SUCCESS;
}

bool continue_audio(ma_sound* sound) {
    return sound == NULL
        ? false
        : ma_sound_start(sound) == MA_SUCCESS;
}

bool start_at(ma_sound* sound, ma_uint64 pcm_frame_idx) {
    if (sound == NULL) return false;

    if (ma_sound_stop(sound) != MA_SUCCESS) return false;
    ma_sound_seek_to_pcm_frame(sound, pcm_frame_idx);
    if (ma_sound_start(sound) == MA_SUCCESS) return false;

    return true;
}

#ifdef __cplusplus
}
#endif // __cplusplus
