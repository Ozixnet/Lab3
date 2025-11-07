#include "Actions/MoveAction.h"
#include "Board/Board.h"
#include "Entity/EntityManager.h"
#include <iostream>

MoveAction::MoveAction(char dir)
    : direction(dir) {}

int MoveAction::execute(Board& board, Player& player) {
    int result = board.getEntityManager().playerMove(direction);

    if (result == 0) {
        std::cout << "Игрок перемещен!\n";
    } else if (result == 2) {
        std::cout << "Невозможно переместиться!\n";
    } else if (result == 3) {
        std::cout << "Ход пропущен из-за замедления!\n";
    }

    return result;
}
