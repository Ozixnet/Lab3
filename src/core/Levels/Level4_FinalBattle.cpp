#include "core/Levels/Level4_FinalBattle.h"
#include "Entity/EntityManager.h"

void Level4_FinalBattle::initialize(EntityManager& em) {
    // Разместить 5 сильных врагов по всему полю
    // HP: 15, Damage: 3
    em.addEnemy(5, 5, 15, 3);     // Враг 1
    em.addEnemy(20, 5, 15, 3);    // Враг 2
    em.addEnemy(5, 20, 15, 3);    // Враг 3
    em.addEnemy(20, 20, 15, 3);   // Враг 4
    em.addEnemy(12, 12, 15, 3);   // Враг 5 (в центре)
    
    // Разместить 3 башни в ключевых позициях
    em.addEnemyTower(10, 10, 10);  // Башня 1 (центр)
    em.addEnemyTower(15, 10, 10);  // Башня 2
    em.addEnemyTower(10, 15, 10);  // Башня 3
    
    // Разместить 2 здания (спавнят врагов каждые 3 хода - быстро!)
    em.addEnemyBuilding(18, 18, 3);  // Здание 1
    em.addEnemyBuilding(22, 22, 3);  // Здание 2
}

bool Level4_FinalBattle::isVictoryCondition(const EntityManager& em) const {
    // Победа: уничтожено ВСЁ - все враги, все башни, все здания
    return em.getAliveEnemyCount() == 0 && 
           em.getTowerCount() == 0 &&
           em.getActiveBuildingCount() == 0;
}

