#include "system.hpp"

#include <array>
#include <memory>
#include <ostream>
#ifdef _WIN32
#include <stdio.h>
#endif


CommandResult run_cmd(const std::string& command) {
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#define WEXITSTATUS
#endif
    std::array<char, 128> buffer;
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(),
                 static_cast<int>(buffer.size()), pipe) != nullptr) {
        result += buffer.data();
    }
    int exitcode = pclose(pipe);
    return CommandResult(result, exitcode);
}
