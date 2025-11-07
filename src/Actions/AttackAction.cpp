#include "Actions/AttackAction.h"
#include "Board/Board.h"
#include "Entity/EntityManager.h"
#include "UI/InputHandler.h"
#include <iostream>

AttackAction::AttackAction(char dir)
    : direction(dir) {}

int AttackAction::execute(Board& board, Player& player) {
    int result = board.getEntityManager().playerAttack(direction);

    if (result == 1) {
        std::cout << "Атака успешна!\n";
        // После атаки двигаем врагов
        board.getEntityManager().moveAllEnemies();
        board.getEntityManager().processBuildingSpawns();
    } else if (result == 0) {
        std::cout << "Атака в пустоту!\n";
    } else {
        std::cout << "Атака заблокирована!\n";
    }

    return result;
}
