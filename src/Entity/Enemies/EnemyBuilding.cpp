#include "Entity/Enemies/EnemyBuilding.h"

EnemyBuilding::EnemyBuilding(int x, int y, int spawnInterval, int id)
    : id(id), x(x), y(y), spawnInterval(spawnInterval),
      currentTimer(spawnInterval), health(10), active(true) {}

bool EnemyBuilding::shouldSpawn() {
    if (!active || health <= 0) return false;

    currentTimer--;

    if (currentTimer <= 0) {
        return true;
    }
    return false;
}

void EnemyBuilding::resetTimer() noexcept {
    currentTimer = spawnInterval;
}
