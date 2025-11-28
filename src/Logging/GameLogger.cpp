#include "Logging/GameLogger.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

// Глобальный файл для отладки — пишем напрямую, без буферизации
static std::ofstream g_debugFile;

void debugLog(const std::string& msg) {
    if (!g_debugFile.is_open()) {
        g_debugFile.open("C:/Users/Arkana/CLionProjects/untitled2/DEBUG_LOG.txt", std::ios::out | std::ios::app);
    }
    if (g_debugFile.is_open()) {
        g_debugFile << msg << std::endl;
    }
}

GameLogger::GameLogger(LogMode mode, const std::string& filename)
    : mode(mode)
    , filename(filename) {
    
    debugLog("=== GameLogger конструктор ===");
    debugLog("Режим: " + std::to_string(static_cast<int>(mode)));
    debugLog("Файл: " + filename);
    
    // Используем АБСОЛЮТНЫЙ путь к логу
    std::string absoluteLogPath = "C:/Users/Arkana/CLionProjects/untitled2/cmake-build-debug/logs/game.log";
    
    std::filesystem::path path(absoluteLogPath);
    if (!path.parent_path().empty()) {
        std::filesystem::create_directories(path.parent_path());
        debugLog("Директория создана: " + path.parent_path().string());
    }

    fileStream.open(absoluteLogPath, std::ios::out | std::ios::app);
    if (!fileStream.is_open()) {
        debugLog("ОШИБКА: Не удалось открыть лог-файл!");
        std::cerr << "⚠️ Не удалось открыть лог-файл: " << absoluteLogPath << "\n";
    } else {
        debugLog("Файл успешно открыт: " + absoluteLogPath);
        fileStream << "\n=== Новая сессия игры [" << formatTimestamp(std::chrono::system_clock::now()) << "] ===" << std::endl;
        std::cout << "📁 Лог-файл открыт: " << absoluteLogPath << "\n";
    }

    EventBus::getInstance().subscribe(this);
    debugLog("Подписка на EventBus выполнена");
    std::cout << "📋 GameLogger подписан на EventBus\n";
}

GameLogger::~GameLogger() {
    EventBus::getInstance().unsubscribe(this);
    if (fileStream.is_open()) {
        fileStream << "=== Конец сессии ===\n";
        fileStream.close();
    }
}

void GameLogger::onEvent(const GameEvent& event) {
    debugLog("onEvent вызван! Тип: " + std::to_string(static_cast<int>(event.getType())));
    
    std::string message = formatTimestamp(event.getTimestamp()) + " " + event.toString();
    
    debugLog("Сообщение: " + message);
    debugLog("fileStream.is_open() = " + std::string(fileStream.is_open() ? "true" : "false"));
    
    // ВСЕГДА пишем в файл
    logToFile(message);
    
    // В консоль пишем только если режим CONSOLE или BOTH
    if (mode == LogMode::CONSOLE || mode == LogMode::BOTH) {
        logToConsole(message);
    }
}

void GameLogger::setMode(LogMode newMode) {
    mode = newMode;
}

void GameLogger::logToConsole(const std::string& message) {
    std::cout << "📋 " << message << "\n";
}

void GameLogger::logToFile(const std::string& message) {
    debugLog("logToFile вызван");
    if (fileStream.is_open()) {
        fileStream << message << std::endl;  // endl гарантирует flush
        debugLog("Записано в game.log: " + message);
    } else {
        debugLog("ОШИБКА: fileStream не открыт!");
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







