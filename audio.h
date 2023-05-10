#ifndef AUDIO_H
#define AUDIO_H

#include "miniaudio.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

bool play_audio(const char* audio_file_path,
                ma_engine* engine,
                ma_sound* sound);

bool stop_audio(ma_sound* sound);

bool continue_audio(ma_sound* sound);

bool continue_audio(ma_sound* sound);

bool start_at(ma_sound* sound, ma_uint64 pcm_frame_idx);

#ifdef __cplusplus
}
#endif // cplusplus

#endif // AUDIO_H
