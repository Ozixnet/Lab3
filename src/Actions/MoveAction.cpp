#include "Actions/MoveAction.h"
#include "Board/Board.h"
#include "Entity/EntityManager.h"
#include "Events/EventBus.h"
#include "Events/GameEvent.h"
#include <iostream>

MoveAction::MoveAction(Direction dir)
    : direction(dir) {}

int MoveAction::execute(Board& board, Player& player) {
    auto [oldX, oldY] = board.getEntityManager().getPlayerCoord();
    int result = board.getEntityManager().playerMove(direction);

    if (result == 0) {
        std::cout << "Игрок перемещен!\n";
        auto [newX, newY] = board.getEntityManager().getPlayerCoord();
        EventBus::getInstance().publish(PlayerMovedEvent(oldX, oldY, newX, newY));
    } else if (result == 2) {
        std::cout << "Невозможно переместиться!\n";
    } else if (result == 3) {
        std::cout << "Ход пропущен из-за замедления!\n";
    }

    return result;
}
