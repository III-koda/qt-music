#include "string.hpp"
#include "../extlib/logger.hpp"

#include <iostream>
#include <regex>
#include <sstream>

void
replace_all_in_place(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) {
        return;
    }
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

std::string
replace_all(const std::string& str, const std::string& from, const std::string& to) {
    std::string res = str;
    replace_all_in_place(res, from, to);
    return res;
}

std::vector<std::string>
split_string(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;

    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

bool
starts_with(const std::string& str, const std::string& prefix) {
    if (str.length() < prefix.length()) {
        return false;
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}

std::string
trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.length() - 1;

    while (start <= end && std::isspace(str[start])) {
        start++;
    }
    while (end > start && std::isspace(str[end])) {
        end--;
    }

    return str.substr(start, end - start + 1);
}

std::string
upper(const std::string& str) {
    std::string res = str;
    std::transform(res.begin(), res.end(), res.begin(), ::toupper);
    return res;
}

std::string lower(const std::string& str) {
    std::string res = str;
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

std::string
get_base_url(const std::string& url) {
    std::regex rgx("^.+?[^\\/:](?=[?\\/]|$)");
    std::smatch match;
    if (std::regex_search(url.begin(), url.end(), match, rgx)) {
        return match[0];
    }
    return "";
}
