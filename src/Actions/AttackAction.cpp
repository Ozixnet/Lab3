#include "Actions/AttackAction.h"
#include "Board/Board.h"
#include "Entity/EntityManager.h"
#include "Events/EventBus.h"
#include "Events/GameEvent.h"
#include <iostream>

AttackAction::AttackAction(char dir)
    : direction(dir) {}

int AttackAction::execute(Board& board, Player& player) {
    int result = board.getEntityManager().playerAttack(direction);

    const auto& attackInfo = board.getEntityManager().getLastPlayerAttackInfo();
    if (attackInfo.hit) {
        EventBus::getInstance().publish(
            DamageDealtEvent("Игрок",
                             attackInfo.targetName.empty() ? "Цель" : attackInfo.targetName,
                             attackInfo.damage));

        if (attackInfo.targetDestroyed) {
            // Очки прокачки выдаются в EntityManager::handleAttackAt()
            EventBus::getInstance().publish(
                EntityDiedEvent(attackInfo.targetName.empty() ? "Цель" : attackInfo.targetName,
                                attackInfo.targetX,
                                attackInfo.targetY));
        }
    }

    if (result == 1) {
        std::cout << "Атака успешна!\n";

        board.getEntityManager().moveAllEnemies();
        board.getEntityManager().processBuildingSpawns();
    } else if (result == 0) {
        std::cout << "Атака в пустоту!\n";
    } else {
        std::cout << "Атака заблокирована!\n";
    }

    return result;
}
