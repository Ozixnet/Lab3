#include "Logging/GameLogger.h"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

GameLogger::GameLogger(LogMode mode, const std::string& filename)
    : mode(mode)
    , filename(filename) {
    if (mode == LogMode::FILE || mode == LogMode::BOTH) {
        std::filesystem::path path(filename);
        if (!path.parent_path().empty()) {
            std::filesystem::create_directories(path.parent_path());
        }

        fileStream.open(filename, std::ios::out | std::ios::app);
        if (!fileStream.is_open()) {
            std::cerr << "⚠️ Не удалось открыть лог-файл: " << filename << "\n";
            this->mode = LogMode::CONSOLE;
        } else {
            fileStream << "\n=== Новая сессия игры ===\n";
        }
    }

    EventBus::getInstance().subscribe(this);
}

GameLogger::~GameLogger() {
    EventBus::getInstance().unsubscribe(this);
    if (fileStream.is_open()) {
        fileStream << "=== Конец сессии ===\n";
        fileStream.close();
    }
}

void GameLogger::onEvent(const GameEvent& event) {
    std::string message = formatTimestamp(event.getTimestamp()) + " " + event.toString();
    switch (mode) {
        case LogMode::CONSOLE:
            logToConsole(message);
            break;
        case LogMode::FILE:
            logToFile(message);
            break;
        case LogMode::BOTH:
            logToConsole(message);
            logToFile(message);
            break;
    }
}

void GameLogger::setMode(LogMode newMode) {
    mode = newMode;
}

void GameLogger::logToConsole(const std::string& message) {
    std::cout << "📋 " << message << "\n";
}

void GameLogger::logToFile(const std::string& message) {
    if (fileStream.is_open()) {
        fileStream << message << "\n";
        fileStream.flush();
    }
}

std::string GameLogger::formatTimestamp(std::chrono::system_clock::time_point time) const {
    auto timeT = std::chrono::system_clock::to_time_t(time);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &timeT);
#else
    tm = *std::localtime(&timeT);
#endif

    std::ostringstream oss;
    oss << "[" << std::put_time(&tm, "%H:%M:%S") << "]";
    return oss.str();
}



