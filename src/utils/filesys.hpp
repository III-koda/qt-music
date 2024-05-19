#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

/**
 * @brief Get the path to the directory storing files and data used for this
 * application.
 */
std::string app_directory();

/** @brief Create a directory if not exist. */
void create_directory(std::string path);

/** @brief Check if a path exists. */
bool exists(std::string path);

void rename_file(const std::string& oldname, const std::string& newname);

void remove_file(const std::string& filename);

std::vector<std::string> files_in_dir(const std::string& dir_path);

bool download_file(const std::string &file_url, const std::string &save_as);

/** @brief Extract the file's name (not including directory) from a path. */
std::string get_file_name(const std::string& filepath,
                          bool include_extension=true);

/** @brief Create a file if not exists. */
bool create_file_if_not_exists(const std::string& file_name);

/** @brief Embed an image into an audio file as metadata. */
bool embed_image_to_audio_file(const std::string& audio_file_path,
                               const std::string& cover_art_path);

#endif
