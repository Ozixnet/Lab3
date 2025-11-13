#include "core/Levels/Level2_TowerDefense.h"
#include "Entity/EntityManager.h"

void Level2_TowerDefense::initialize(EntityManager& em) {
    // Разместить 4 средних врага
    // HP: 8, Damage: 2
    em.addEnemy(4, 4, 8, 2);   // Враг 1
    em.addEnemy(10, 4, 8, 2);  // Враг 2
    em.addEnemy(4, 10, 8, 2);  // Враг 3
    em.addEnemy(10, 10, 8, 2); // Враг 4
    
    // Разместить 1 башню в центре
    // HP: 10
    em.addEnemyTower(7, 7, 10);   // Башня в центре
    
    // Нет зданий
}

bool Level2_TowerDefense::isVictoryCondition(const EntityManager& em) const {
    // Победа: все враги мертвы И все башни уничтожены
    return em.getAliveEnemyCount() == 0 && em.getTowerCount() == 0;
}

