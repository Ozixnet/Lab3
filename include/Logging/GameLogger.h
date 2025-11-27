#pragma once

#include "Events/EventBus.h"
#include <chrono>
#include <fstream>
#include <string>

enum class LogMode {
    CONSOLE,
    FILE,
    BOTH
};

class GameLogger : public IEventListener {
public:
    GameLogger(LogMode mode, const std::string& filename = "logs/game.log");
    ~GameLogger() override;

    void onEvent(const GameEvent& event) override;
    void setMode(LogMode newMode);

private:
    void logToConsole(const std::string& message);
    void logToFile(const std::string& message);
    std::string formatTimestamp(std::chrono::system_clock::time_point time) const;

private:
    LogMode mode;
    std::ofstream fileStream;
    std::string filename;
};

