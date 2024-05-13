#ifndef CPPLOGGER_HPP
#define CPPLOGGER_HPP

#include <chrono>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger {
public:
    static Logger* get_instance() {
        static Logger instance;
        return &instance;
    }

    void log(LogLevel log_level, const std::string& message) {
        std::string log_message = get_current_timestamp_str() +
                                  get_log_level_str(log_level) +
                                  message;
        log_to_file(log_message);
    }

    void set_log_filepath(const std::string& filepath) {
        _log_filepath = filepath;
    }

private:
    std::string _log_filepath;

    std::string get_log_level_str(LogLevel level) const {
        switch (level) {
        case LogLevel::ERROR:
            return "[ERROR]  ";
        case LogLevel::WARNING:
            return "[WARN]   ";
        case LogLevel::INFO:
            return "[INFO]   ";
        case LogLevel::DEBUG:
            return "[DEBUG]  ";
        }
    }


    bool log_to_file(const std::string& log_message) {
        std::ofstream my_log_file;
        my_log_file.open(_log_filepath, std::ios::app);
        if (!my_log_file.is_open()) {
            return false;
        }

        my_log_file << log_message << std::endl;
        my_log_file.close();
    }

    std::string get_current_timestamp_str() const {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_c);
        std::ostringstream timestamp_stream;
        timestamp_stream << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
        return "[" + timestamp_stream.str() + "]";
    }
};


#endif // CPPLOGGER_HPP
