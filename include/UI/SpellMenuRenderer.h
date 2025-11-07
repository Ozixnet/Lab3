#pragma once
#include <memory>
#include "../Actions/GameAction.h"

class Board;
class Hand;

class InputHandler {
public:
    static std::unique_ptr<GameAction> handleMovementInput(char direction);
    static std::unique_ptr<GameAction> handleAttackInput(Board* board);
    static std::unique_ptr<GameAction> handleSpellInput(Hand* hand, Board* board);
    static std::unique_ptr<GameAction> handleOptionsMenu(Board* board, Hand* hand);
};
