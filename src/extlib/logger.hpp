#ifndef CPPLOGGER_HPP
#define CPPLOGGER_HPP

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <sstream>

#include <QDebug>

#include "../utils/system.hpp"

#ifdef IS_LINUX
#include <sys/file.h>
#endif // IS_LINUX


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
                                  get_log_level_str(log_level) + " " +
                                  message;
        log_to_file(log_message);
    }

    void set_log_filepath(const std::string& filepath) {
        _log_filepath = filepath;
    }

private:
    static std::string get_log_level_str(LogLevel level) {
        switch (level) {
        case LogLevel::ERROR:
            return "[ERROR]";
        case LogLevel::WARNING:
            return "[WARN]";
        case LogLevel::INFO:
            return "[INFO]";
        case LogLevel::DEBUG:
            return "[DEBUG]";
        }
        return ""; // Should never reach here
    }

    bool log_to_file(const std::string& log_message) {
        _mtx.lock();
        int logfd = lock_file();
        if (logfd == -1) {
            qDebug() << "Failed to lock log file";
            return false;
        }
        std::ofstream my_log_file;
        my_log_file.open(_log_filepath, std::ios::app);
        if (!my_log_file.is_open()) {
            return false;
        }

        my_log_file << log_message << std::endl;
        my_log_file.close();
        if (!unlock_file(logfd)) {
            qDebug() << "Failed to unlock log file";
            return false;
        }
        _mtx.unlock();
        return true;
    }

    std::string get_current_timestamp_str() const {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_c);
        std::ostringstream timestamp_stream;
        timestamp_stream << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
        return "[" + timestamp_stream.str() + "]";
    }

    int lock_file() {
        if (is_linux()) {
            const char* file_path = _log_filepath.c_str();
            int logfd = open(file_path, O_RDONLY);
            if (logfd == -1) {
                qDebug() << "Failed open log file: " << file_path;
                return -1;
            }
            if (flock(logfd, LOCK_EX) == -1) {
                qDebug() << "Failed to lock log file " << file_path;
                return -1;
            }
            return logfd;
        }
        return 0;
    }

    bool unlock_file(int logfd) {
        if (is_linux()) {
            const char* file_path = _log_filepath.c_str();
            if (logfd == -1) {
                qDebug() << "Failed to open log file" << file_path;
                return false;
            }
            return flock(logfd, LOCK_UN) == 0;
        }
        return true;
    }

    std::string _log_filepath;
    std::mutex _mtx;
};


#endif // CPPLOGGER_HPP
