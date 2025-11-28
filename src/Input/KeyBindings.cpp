#include "Input/KeyBindings.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

namespace {

std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

} // namespace

KeyBindings::KeyBindings() {
    setDefaults();
}

void KeyBindings::setDefaults() {
    keyToCommand.clear();
    commandToKey.clear();

    keyToCommand["w"] = GameCommand::MOVE_UP;
    keyToCommand["s"] = GameCommand::MOVE_DOWN;
    keyToCommand["a"] = GameCommand::MOVE_LEFT;
    keyToCommand["d"] = GameCommand::MOVE_RIGHT;
    keyToCommand["o"] = GameCommand::OPEN_MENU;
    keyToCommand["q"] = GameCommand::QUIT;
    keyToCommand["quit"] = GameCommand::QUIT;
    keyToCommand["exit"] = GameCommand::QUIT;
    keyToCommand["save"] = GameCommand::SAVE;
    keyToCommand["load"] = GameCommand::LOAD;

    commandToKey[GameCommand::MOVE_UP] = "w";
    commandToKey[GameCommand::MOVE_DOWN] = "s";
    commandToKey[GameCommand::MOVE_LEFT] = "a";
    commandToKey[GameCommand::MOVE_RIGHT] = "d";
    commandToKey[GameCommand::OPEN_MENU] = "o";
    commandToKey[GameCommand::QUIT] = "q";
    commandToKey[GameCommand::SAVE] = "save";
    commandToKey[GameCommand::LOAD] = "load";

    std::cout << "✅ Загружены дефолтные настройки управления\n";
}

bool KeyBindings::loadFromFile(const std::string& filename) {
    std::cout << "[DEBUG KeyBindings] Попытка загрузить из: " << filename << "\n";
    std::cout << "[DEBUG] Текущая рабочая директория: " << std::filesystem::current_path().string() << "\n";
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[DEBUG] ✗ Файл конфигурации '" << filename
                  << "' не найден. Используются дефолтные настройки.\n";
        setDefaults();
        return false;
    }
    
    std::cout << "[DEBUG] ✓ Файл открыт успешно\n";

    std::map<std::string, GameCommand> tempKeyToCommand;
    std::unordered_map<GameCommand, std::string> tempCommandToKey;

    std::string line;
    int lineNumber = 0;
    bool hasError = false;

    while (std::getline(file, line)) {
        lineNumber++;

        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        GameCommand cmd;
        std::string key;
        if (!parseLine(line, cmd, key)) {
            std::cerr << "❌ Ошибка в строке " << lineNumber << ": '" << line << "'\n";
            hasError = true;
            continue;
        }

        std::string lowerKey = toLower(key);
        auto existing = tempKeyToCommand.find(lowerKey);
        if (existing != tempKeyToCommand.end() && existing->second != cmd) {
            std::cerr << "❌ Клавиша '" << lowerKey << "' назначена нескольким командам!\n";
            hasError = true;
            continue;
        }

        tempKeyToCommand[lowerKey] = cmd;
        tempCommandToKey[cmd] = lowerKey;
        std::cout << "[DEBUG] Загружено: " << commandToString(cmd) << " = '" << lowerKey << "'\n";
    }

    file.close();
    std::cout << "[DEBUG] Загружено всего привязок: " << tempKeyToCommand.size() << "\n";

    if (hasError) {
        std::cerr << "⚠️ Обнаружены ошибки в конфигурации. Используются дефолтные настройки.\n";
        setDefaults();
        return false;
    }

    keyToCommand = std::move(tempKeyToCommand);
    commandToKey = std::move(tempCommandToKey);

    keyToCommand["quit"] = GameCommand::QUIT;
    keyToCommand["exit"] = GameCommand::QUIT;

    if (!validate()) {
        std::cerr << "⚠️ Конфигурация не прошла валидацию. Используются дефолтные настройки.\n";
        setDefaults();
        return false;
    }

    std::cout << "\n";
    std::cout << "═══════════════════════════════════════\n";
    std::cout << "✅ Настройки управления загружены из '" << filename << "'\n";
    std::cout << "═══════════════════════════════════════\n";
    return true;
}

bool KeyBindings::parseLine(const std::string& line, GameCommand& cmd, std::string& key) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false;
    }

    std::string cmdStr = line.substr(0, pos);
    key = line.substr(pos + 1);

    cmdStr.erase(0, cmdStr.find_first_not_of(" \t"));
    cmdStr.erase(cmdStr.find_last_not_of(" \t") + 1);
    key.erase(0, key.find_first_not_of(" \t"));
    key.erase(key.find_last_not_of(" \t") + 1);

    if (cmdStr.empty() || key.empty()) {
        return false;
    }

    cmd = stringToCommand(cmdStr);
    if (cmd == GameCommand::NONE) {
        return false;
    }

    return true;
}

GameCommand KeyBindings::stringToCommand(const std::string& cmdStr) const {
    if (cmdStr == "MOVE_UP") return GameCommand::MOVE_UP;
    if (cmdStr == "MOVE_DOWN") return GameCommand::MOVE_DOWN;
    if (cmdStr == "MOVE_LEFT") return GameCommand::MOVE_LEFT;
    if (cmdStr == "MOVE_RIGHT") return GameCommand::MOVE_RIGHT;
    if (cmdStr == "OPEN_MENU") return GameCommand::OPEN_MENU;
    if (cmdStr == "QUIT") return GameCommand::QUIT;
    if (cmdStr == "SAVE") return GameCommand::SAVE;
    if (cmdStr == "LOAD") return GameCommand::LOAD;
    return GameCommand::NONE;
}

std::string KeyBindings::commandToString(GameCommand cmd) const {
    switch (cmd) {
        case GameCommand::MOVE_UP: return "MOVE_UP";
        case GameCommand::MOVE_DOWN: return "MOVE_DOWN";
        case GameCommand::MOVE_LEFT: return "MOVE_LEFT";
        case GameCommand::MOVE_RIGHT: return "MOVE_RIGHT";
        case GameCommand::OPEN_MENU: return "OPEN_MENU";
        case GameCommand::QUIT: return "QUIT";
        case GameCommand::SAVE: return "SAVE";
        case GameCommand::LOAD: return "LOAD";
        default: return "NONE";
    }
}

bool KeyBindings::validate() const {
    if (hasDuplicateKeys()) {
        std::cerr << "❌ Найдены дубликаты клавиш!\n";
        return false;
    }

    if (!hasAllRequiredCommands()) {
        std::cerr << "❌ Отсутствуют обязательные команды!\n";
        return false;
    }

    return true;
}

bool KeyBindings::hasDuplicateKeys() const {
    std::unordered_map<GameCommand, std::unordered_set<std::string>> commandKeys;

    for (const auto& [key, cmd] : keyToCommand) {
        if (key == "quit" || key == "exit") {
            continue;
        }
        commandKeys[cmd].insert(key);
    }

    for (const auto& [cmd, keys] : commandKeys) {
        if (keys.size() > 1) {
            std::cerr << "❌ Команда '" << commandToString(cmd)
                      << "' назначена на несколько клавиш!\n";
            return true;
        }
    }

    return false;
}

bool KeyBindings::hasAllRequiredCommands() const {
    std::vector<GameCommand> required = {
        GameCommand::MOVE_UP,
        GameCommand::MOVE_DOWN,
        GameCommand::MOVE_LEFT,
        GameCommand::MOVE_RIGHT,
        GameCommand::OPEN_MENU,
        GameCommand::QUIT
    };

    for (GameCommand cmd : required) {
        if (commandToKey.find(cmd) == commandToKey.end()) {
            std::cerr << "❌ Отсутствует команда: " << commandToString(cmd) << "\n";
            return false;
        }
    }

    return true;
}

GameCommand KeyBindings::getCommand(const std::string& input) const {
    std::string lowerInput = toLower(input);
    std::cout << "[DEBUG KeyBindings::getCommand] Поиск команды для ввода: '" << input 
              << "' (lowercase: '" << lowerInput << "')\n";
    std::cout << "[DEBUG] Всего привязок в keyToCommand: " << keyToCommand.size() << "\n";
    
    auto it = keyToCommand.find(lowerInput);
    if (it != keyToCommand.end()) {
        std::cout << "[DEBUG] ✓ Команда найдена: " << static_cast<int>(it->second) << "\n";
        return it->second;
    }
    
    std::cout << "[DEBUG] ✗ Команда НЕ найдена для '" << lowerInput << "'\n";
    std::cout << "[DEBUG] Доступные клавиши: ";
    for (const auto& [key, cmd] : keyToCommand) {
        std::cout << "'" << key << "' ";
    }
    std::cout << "\n";
    
    return GameCommand::NONE;
}

std::string KeyBindings::getKeyForCommand(GameCommand cmd) const {
    auto it = commandToKey.find(cmd);
    if (it != commandToKey.end()) {
        return it->second;
    }
    return "?";
}

void KeyBindings::printBindings() const {
    std::cout << "\n╔═══════════════════════════════════════╗\n";
    std::cout << "║       ТЕКУЩИЕ НАСТРОЙКИ УПРАВЛЕНИЯ     ║\n";
    std::cout << "╠═══════════════════════════════════════╣\n";

    for (const auto& [cmd, key] : commandToKey) {
        std::cout << "║ " << commandToString(cmd) << " = " << key << "\n";
    }

    std::cout << "╚═══════════════════════════════════════╝\n";
}

