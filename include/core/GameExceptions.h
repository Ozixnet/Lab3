#pragma once
#include <stdexcept>
#include <string>

// Базовое исключение для игровых операций
class GameException : public std::runtime_error {
public:
    explicit GameException(const std::string& message, const std::string& location = "")
        : std::runtime_error(message), location(location) {}
    
    const std::string& getLocation() const { return location; }
    
private:
    std::string location;
};

// Исключение для ошибок сохранения
class SaveException : public GameException {
public:
    explicit SaveException(const std::string& message, const std::string& location = "")
        : GameException("Save error: " + message, location) {}
};

// Исключение для ошибок загрузки
class LoadException : public GameException {
public:
    explicit LoadException(const std::string& message, const std::string& location = "")
        : GameException("Load error: " + message, location) {}
};

// Исключение для некорректных данных
class InvalidDataException : public LoadException {
public:
    explicit InvalidDataException(const std::string& message, const std::string& location = "")
        : LoadException("Invalid data: " + message, location) {}
};

// Исключение для отсутствия файла
class FileNotFoundException : public LoadException {
public:
    explicit FileNotFoundException(const std::string& filename, const std::string& location = "")
        : LoadException("File not found: " + filename, location) {}
};

