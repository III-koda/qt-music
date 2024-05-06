#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>


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
