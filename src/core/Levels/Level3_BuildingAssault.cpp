#include "core/Levels/Level3_BuildingAssault.h"
#include "Entity/EntityManager.h"

void Level3_BuildingAssault::initialize(EntityManager& em) {
    // Разместить 3 начальных врага
    // HP: 10, Damage: 2
    em.addEnemy(5, 5, 10, 2);   // Враг 1
    em.addEnemy(10, 8, 10, 2);  // Враг 2
    em.addEnemy(8, 12, 10, 2);  // Враг 3
    
    // Разместить 1 башню для защиты базы
    em.addEnemyTower(15, 15, 10);
    
    // Разместить 1 здание (спавнит врага каждые 5 ходов)
    // Параметры: x, y, spawnInterval
    em.addEnemyBuilding(17, 17, 5);
}

bool Level3_BuildingAssault::isVictoryCondition(const EntityManager& em) const {
    // Победа: все враги мертвы, башня уничтожена И здание разрушено
    return em.getAliveEnemyCount() == 0 && 
           em.getTowerCount() == 0 &&
           em.getActiveBuildingCount() == 0;
}

