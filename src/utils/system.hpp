#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>

#if defined(__linux__) || defined(linux) || defined(__linux)
#define IS_LINUX
#endif

#if defined(unix) || defined(__unix) || defined(__unix__)
#define IS_UNIX
#endif

#if defined(__APPLE__) || defined(__MACH__)
#define IS_MAC
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__)
#define IS_WIN
#endif

inline bool is_linux() {
#ifdef IS_LINUX
    return true;
#else
    return false;
#endif
}

inline bool is_unix() {
#ifdef IS_UNIX
    return true;
#else
    return false;
#endif
}

inline bool is_mac() {
#ifdef IS_MAC
    return true;
#else
    return false;
#endif
}

inline bool is_windows() {
#ifdef IS_WIN
    return true;
#else
    return false;
#endif
}


class CommandResult {
public:
    CommandResult() : m_output(""), m_exitstatus(-1) {}

    CommandResult(std::string output, int exit_status) :
            m_output(output), m_exitstatus(exit_status) {}

    std::string output() const { return m_output; }
    int exit_status() const { return m_exitstatus; }

    bool operator==(const CommandResult &rhs) const {
        return m_output == rhs.m_output && m_exitstatus == rhs.m_exitstatus;
    }

    bool operator!=(const CommandResult &rhs) const {
        return !(rhs == *this);
    }

private:
    std::string m_output;
    int m_exitstatus;
};


/**
 * Execute system command and get STDOUT result.
 * Regular system() only gives back exit status, this gives back output as well.
 *
 * @param command system command to execute
 *
 * @return commandResult containing STDOUT (not stderr) output & exitstatus
 * of command. Empty if command failed (or has no output). If you want stderr,
 * use shell redirection (2&>1).
 */
CommandResult run_cmd(const std::string& command);

#endif
