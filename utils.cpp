#include "utils.h"

#ifndef __has_include
static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#  endif
#endif

std::vector<std::string> files_in_dir(const std::string& dir_path) {
    std::vector<std::string> result;

    for (const auto & entry : fs::directory_iterator(dir_path)) {
        if (entry.is_directory()) continue;

        std::string filepath = entry.path().string();
        replace(filepath, "\\", "/");

        result.push_back(filepath);
    }
    return result;
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos) return false;

    str.replace(start_pos, from.length(), to);
    return true;
}
