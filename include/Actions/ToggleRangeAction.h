#ifndef TOGGLE_RANGE_ACTION_H
#define TOGGLE_RANGE_ACTION_H

#include "GameAction.h"

class ToggleRangeAction : public GameAction {
public:
    ToggleRangeAction() = default;
    int execute(Board& board, Player& player) override;
};

#endif // TOGGLE_RANGE_ACTION_H
