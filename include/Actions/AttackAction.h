#ifndef ATTACK_ACTION_H
#define ATTACK_ACTION_H

#include "GameAction.h"

/**
 * @brief Действие атаки игрока
 * 
 * Создаётся через IInputReader::parseCommand() (требование 1).
 * Публикует DamageDealtEvent и EntityDiedEvent в EventBus для логирования (требование 6).
 * 
 * @see GameAction
 * @see IInputReader
 * @see EventBus
 */
class AttackAction : public GameAction {
private:
    char direction;

public:
    explicit AttackAction(char dir);
    int execute(Board& board, Player& player) override;
};

#endif // ATTACK_ACTION_H
