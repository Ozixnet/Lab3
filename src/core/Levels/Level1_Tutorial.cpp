#include "core/Levels/Level1_Tutorial.h"
#include "Entity/EntityManager.h"

void Level1_Tutorial::initialize(EntityManager& em) {
    // Разместить 3 слабых врага в разных местах
    // HP: 5, Damage: 1
    em.addEnemy(3, 3, 5, 1);  // Враг 1 (верхний левый)
    em.addEnemy(7, 3, 5, 1);  // Враг 2 (верхний правый)
    em.addEnemy(5, 7, 5, 1);  // Враг 3 (нижний центр)
    
    // Никаких башен и зданий для обучающего уровня
}

bool Level1_Tutorial::isVictoryCondition(const EntityManager& em) const {
    // Победа: все враги мертвы
    return em.getAliveEnemyCount() == 0;
}

