#ifndef MOVE_ACTION_H
#define MOVE_ACTION_H

#include "GameAction.h"
#include "Input/Direction.h"

/**
 * @brief Действие движения игрока
 * 
 * Создаётся через IInputReader::parseCommand() (требование 1).
 * Публикует PlayerMovedEvent в EventBus для логирования (требование 6).
 * 
 * Использует Direction вместо char для типобезопасности.
 * 
 * @see GameAction
 * @see IInputReader
 * @see EventBus
 * @see Direction
 */
class MoveAction : public GameAction {
private:
    Direction direction;

public:
    explicit MoveAction(Direction dir);
    int execute(Board& board, Player& player) override;
    
    Direction getDirection() const { return direction; }
};

#endif // MOVE_ACTION_H
