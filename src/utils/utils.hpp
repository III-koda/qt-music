#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

std::vector<std::string> files_in_dir(const std::string& dir_path);

bool replace(std::string& str, const std::string& from, const std::string& to);

#endif
