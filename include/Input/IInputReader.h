#pragma once

#include <memory>
#include <string>

#include "Input/GameCommand.h"

class GameAction;
class Board;
class Hand;
class Player;
class IGameRenderer;

/**
 * @brief Интерфейс для считывания и преобразования ввода пользователя в объект команды
 * 
 * @requirement Требование 1: Создать класс считывающий ввод пользователя и преобразующий ввод пользователь в объект команды
 * 
 * Реализации:
 * - ConsoleInputReader - консольный ввод
 * - GuiInputReader - GUI ввод
 * 
 * @see ConsoleInputReader
 * @see GuiInputReader
 * @see GameAction
 * @see GameCommand
 */
class IInputReader {
public:
    virtual ~IInputReader() = default;

    virtual void setRenderer(IGameRenderer* renderer) = 0;
    virtual GameCommand mapInputToCommand(const std::string& input) const = 0;

    // Считывает строку ввода от пользователя
    virtual std::string readRawInput() = 0;

    // Преобразует строку в команду (GameAction)
    virtual std::unique_ptr<GameAction> parseCommand(
        const std::string& input,
        Board* board,
        Hand* hand,
        Player* player
    ) = 0;
};

