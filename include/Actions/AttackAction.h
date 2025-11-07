#ifndef ATTACK_ACTION_H
#define ATTACK_ACTION_H

#include "GameAction.h"

class AttackAction : public GameAction {
private:
    char direction;

public:
    explicit AttackAction(char dir);
    int execute(Board& board, Player& player) override;
};

#endif // ATTACK_ACTION_H
