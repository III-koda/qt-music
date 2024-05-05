#ifndef YTDLP_HPP
#define YTDLP_HPP

#include <string>

bool download_ytdlp();

/**
 * @brief Download audio (mp3) from a given Youtube URL.
 *
 * @param url           The URL of the video to be downloaded as mp3.
 * @param save_dir      The directory where the file will be downloaded to.
 *
 * @return True if downloaded successfully, false otherwise.
 */
bool download_youtube_audio(std::string url, std::string save_dir);

#endif
