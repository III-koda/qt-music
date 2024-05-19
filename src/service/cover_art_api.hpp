#ifndef COVER_ART_API
#define COVER_ART_API

#include <string>

#include "../modules/songs_manager/isong.hpp"
#include "../utils/string.hpp"

std::string get_cover_art(ISongData song_data);

#endif
