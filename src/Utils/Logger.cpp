#include "Utils/Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>

std::string getLevelString(LogLevel level) {
    switch (level) {
    case LogLevel::DEBUG: return "[DEBUG]";
    case LogLevel::INFO: return "[INFO]";
    case LogLevel::WARNING: return "[WARNING]";
    case LogLevel::ERROR: return "[ERROR]";
    default: return "[UNKNOWN]";
    }
}

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%H:%M:%S");
    return oss.str();
}

void Logger::log(LogLevel level, const std::string& message) {
    std::cout << "[" << getCurrentTime() << "] "
              << getLevelString(level) << " "
              << message << "\n";
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}
