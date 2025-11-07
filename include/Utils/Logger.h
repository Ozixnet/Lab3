#ifndef LOGGER_H
#define LOGGER_H

#include <string>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    // Логировать сообщение
    static void log(LogLevel level, const std::string& message);
    
    // Сокращенные версии
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);

private:
    Logger() = delete;  // Утилитарный класс
};

#endif // LOGGER_H
