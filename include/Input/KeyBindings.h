#pragma once

#include "Input/GameCommand.h"
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * Класс для управления привязками клавиш к командам.
 */
class KeyBindings {
public:
    KeyBindings();

    bool loadFromFile(const std::string& filename);
    void setDefaults();

    GameCommand getCommand(const std::string& input) const;
    std::string getKeyForCommand(GameCommand cmd) const;
    void printBindings() const;

private:
    bool parseLine(const std::string& line, GameCommand& cmd, std::string& key);
    GameCommand stringToCommand(const std::string& cmdStr) const;
    std::string commandToString(GameCommand cmd) const;
    bool validate() const;
    bool hasDuplicateKeys() const;
    bool hasAllRequiredCommands() const;

private:
    std::map<std::string, GameCommand> keyToCommand;
    std::unordered_map<GameCommand, std::string> commandToKey;
};







