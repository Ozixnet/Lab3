#ifndef MOVE_ACTION_H
#define MOVE_ACTION_H

#include "GameAction.h"

class MoveAction : public GameAction {
private:
    char direction;

public:
    explicit MoveAction(char dir);
    int execute(Board& board, Player& player) override;
};

#endif // MOVE_ACTION_H
