#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include <string>
#include <vector>

void replace_all_in_place(std::string& str, const std::string& from, const std::string& to);

std::string replace_all(const std::string& str, const std::string& from, const std::string& to);

std::vector<std::string> split_string(const std::string& str, char delimiter);

bool starts_with(const std::string& str, const std::string& prefix);

std::string trim(const std::string& str);

std::string get_base_url(const std::string& url);

#endif
