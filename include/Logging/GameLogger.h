#pragma once

#include "Events/EventBus.h"
#include <chrono>
#include <fstream>
#include <string>

enum class LogMode {
    CONSOLE,  ///< Логи только в консоль
    FILE,     ///< Логи только в файл
    BOTH      ///< Логи и в консоль, и в файл
};

/**
 * @brief Система логирования событий в игре
 * 
 * @requirement Требование 6: Добавить систему логирования событий в игре
 * 
 * Система реагирует на игровые события (урон, движение, заклинания и т.д.)
 * и записывает об этом событии.
 * 
 * Игровые сущности не должны напрямую вызывать систему логировать,
 * а только лишь информировать о событии через EventBus.
 * 
 * Запись может идти как в файл, так и в терминал,
 * способ логирования определяется пользователем через параметры запуска программы.
 * 
 * @see EventBus
 * @see IEventListener
 * @see GameEvent
 */
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

