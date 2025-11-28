#include "Entity/EntityManager.h"
#include "Magic/TrapRegistry.h"
#include "Entity/Allies/Ally.h"
#include <iostream>
#include <algorithm>
#include "Board/Board.h"


EntityManager::EntityManager(Board* boardPtr, int gridSize, Player* playerPtr)
    : board(boardPtr), gridSize(gridSize), player(playerPtr) {
}

EntityManager::EntityManager(const EntityManager& other)
    : board(other.board), gridSize(other.gridSize), player(other.player),
      plCoord(other.plCoord), enemies(other.enemies),
      buildings(other.buildings), towers(other.towers),
      allies(other.allies), allyCoords(other.allyCoords),
      buffContext(other.buffContext) {}

EntityManager::EntityManager(EntityManager&& other) noexcept
    : board(other.board), gridSize(other.gridSize), player(other.player),
      plCoord(std::move(other.plCoord)), enemies(std::move(other.enemies)),
      buildings(std::move(other.buildings)), towers(std::move(other.towers)),
      allies(std::move(other.allies)), allyCoords(std::move(other.allyCoords)),
      buffContext(std::move(other.buffContext)) {}

EntityManager& EntityManager::operator=(const EntityManager& other) {
    if (this != &other) {
        board = other.board;
        gridSize = other.gridSize;
        player = other.player;
        plCoord = other.plCoord;
        enemies = other.enemies;
        buildings = other.buildings;
        towers = other.towers;
        allies = other.allies;
        allyCoords = other.allyCoords;
        buffContext = other.buffContext;
    }
    return *this;
}

EntityManager& EntityManager::operator=(EntityManager&& other) noexcept {
    if (this != &other) {
        board = other.board;
        gridSize = other.gridSize;
        player = other.player;
        plCoord = std::move(other.plCoord);
        enemies = std::move(other.enemies);
        buildings = std::move(other.buildings);
        towers = std::move(other.towers);
        allies = std::move(other.allies);
        allyCoords = std::move(other.allyCoords);
        buffContext = std::move(other.buffContext);
    }
    return *this;
}

void EntityManager::beginPlayerAttackRecord() {
    recordingPlayerAttack = true;
    lastPlayerAttackInfo = AttackInfo{};
}

void EntityManager::endPlayerAttackRecord() {
    recordingPlayerAttack = false;
}

void EntityManager::recordPlayerAttackHit(const std::string& targetName,
                                          int x,
                                          int y,
                                          int damage,
                                          bool destroyed) {
    if (!recordingPlayerAttack) {
        return;
    }

    lastPlayerAttackInfo.hit = true;
    lastPlayerAttackInfo.targetName = targetName;
    lastPlayerAttackInfo.targetX = x;
    lastPlayerAttackInfo.targetY = y;
    lastPlayerAttackInfo.damage = damage;
    lastPlayerAttackInfo.targetDestroyed = destroyed;
}


// ========================================
// ИГРОВАЯ ЛОГИКА
// ========================================

std::pair<std::vector<int>, std::vector<int>> EntityManager::enemyScanner(int x, int y, int gridSize) const
{
    Grid& grid = board->getGrid();

    std::vector<int> indexEnemies;
    std::vector<int> indexBuildings;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
        {
            if (x <= 0 || x >= gridSize - 1 || y <= 0 || y >= gridSize - 1) {
                continue;
            }
            CellType cellType = grid.getCell(x, y);
            if (cellType == CellType::Enemy)
            {
                int enemyIndex = findEnemyAtPosition(x, y);
                indexEnemies.push_back(enemyIndex);
            }

            if (cellType == CellType::HellBuild) {
                int enemyIndex = findBuildingAtPosition(x, y);
                indexBuildings.push_back(enemyIndex);
            }
        }
    }

    return std::make_pair(indexEnemies, indexBuildings);
}

int EntityManager::handleAttackAt(int x, int y, int damage) {
    // Проверка границ
    if (x < 0 || x >= gridSize || y < 0 || y >= gridSize) {
        std::cout << "[WARNING] handleAttackAt: координаты вне границ (" << x << ", " << y << ")" << std::endl;
        return -1;
    }
    
    Grid& grid = board->getGrid();
    CellType cellType = grid.getCell(x, y);
    int towerIndex = findTowerAtPosition(x, y);

    // Атака по игроку (для башен)
    if (cellType == CellType::Player) {
        auto [playerX, playerY] = getPlayerCoord();
        if (playerX == x && playerY == y) {
            player->ReduceHealth(damage);
            std::cout << "⚔ Игрок получил " << damage << " урона от башни! HP: " 
                      << player->GetHealth() << std::endl;
            return 0; // Попал по игроку
        }
    }

    // Атака по врагу
    if (cellType == CellType::Enemy) {
        int enemyIndex = findEnemyAtPosition(x, y);
        if (enemyIndex >= 0 && enemyIndex < static_cast<int>(enemies.size())) {
            int oldHealth = enemies[enemyIndex].GetHealth();
            enemies[enemyIndex].ReduceHealth(damage);

            std::cout << "Попадание по врагу #" << enemies[enemyIndex].GetID()
                      << "! Урон: " << damage
                      << " | Было HP: " << oldHealth
                      << " | Осталось HP: " << enemies[enemyIndex].GetHealth() << std::endl;

            const std::string targetName = "Враг #" + std::to_string(enemies[enemyIndex].GetID());
            bool destroyed = !enemies[enemyIndex].IsAlive();
            recordPlayerAttackHit(targetName, x, y, damage, destroyed);

            if (!enemies[enemyIndex].IsAlive()) {
                std::cout << "💀 Враг #" << enemies[enemyIndex].GetID() << " повержен!" << std::endl;
                // 🎯 Выдаём очко прокачки за убийство врага
                player->AddUpgradePoints(1);
                std::cout << "⭐ +1 очко прокачки!\n";
                removeEnemy(enemyIndex);
                return 1; // Убит враг
            }
            return 0; // Попал, но не убил
        }
    }

    // Атака по зданию
    if (cellType == CellType::HellBuild) {
        int buildingIndex = findBuildingAtPosition(x, y);
        std::cout << "[DEBUG] Атака по зданию: cellType=HellBuild, buildingIndex=" << buildingIndex 
                  << ", buildings.size()=" << buildings.size() << std::endl;
        
        if (buildingIndex >= 0 && buildingIndex < static_cast<int>(buildings.size())) {
            int oldHealth = buildings[buildingIndex].getHealth();
            buildings[buildingIndex].takeDamage(damage);

            std::cout << "Попадание по зданию #" << buildings[buildingIndex].getID()
                      << "! Урон: " << damage
                      << " | Было HP: " << oldHealth
                      << " | Осталось HP: " << buildings[buildingIndex].getHealth() << std::endl;

            const std::string targetName = "Здание #" + std::to_string(buildings[buildingIndex].getID());
            bool destroyed = buildings[buildingIndex].getHealth() <= 0;
            recordPlayerAttackHit(targetName, x, y, damage, destroyed);

            if (buildings[buildingIndex].getHealth() <= 0) {
                std::cout << "💥 Здание #" << buildings[buildingIndex].getID() << " разрушено!" << std::endl;
                // 🎯 Выдаём 2 очка прокачки за уничтожение здания
                player->AddUpgradePoints(2);
                std::cout << "⭐ +2 очка прокачки!\n";
                removeBuilding(buildingIndex);  // <-- УДАЛЯЕМ вместо setActive(false)
                return 2; // Уничтожено здание
            }
            return 0; // Попал, но не уничтожил
        } else {
            std::cout << "[WARNING] Здание не найдено по координатам (" << x << ", " << y 
                      << "), но cellType=HellBuild. Возможно, здание было удалено или координаты не совпадают." << std::endl;
            // Попробуем найти здание через прямой поиск
            for (size_t i = 0; i < buildings.size(); ++i) {
                std::cout << "[DEBUG] Здание #" << i << " на (" << buildings[i].getX() 
                          << ", " << buildings[i].getY() << "), HP=" << buildings[i].getHealth() << std::endl;
            }
        }
    } else {
        // Проверяем, есть ли здание на этой позиции, даже если cellType не HellBuild
        int buildingIndex = findBuildingAtPosition(x, y);
        if (buildingIndex >= 0 && buildingIndex < static_cast<int>(buildings.size())) {
            std::cout << "[WARNING] Здание найдено на (" << x << ", " << y 
                      << "), но cellType=" << static_cast<int>(cellType) 
                      << " (не HellBuild). Исправляем Grid..." << std::endl;
            // Исправляем Grid - восстанавливаем правильный тип клетки
            grid.setCell(x, y, CellType::HellBuild);
            // Повторяем атаку
            int oldHealth = buildings[buildingIndex].getHealth();
            buildings[buildingIndex].takeDamage(damage);

            std::cout << "Попадание по зданию #" << buildings[buildingIndex].getID()
                      << "! Урон: " << damage
                      << " | Было HP: " << oldHealth
                      << " | Осталось HP: " << buildings[buildingIndex].getHealth() << std::endl;

            const std::string targetName = "Здание #" + std::to_string(buildings[buildingIndex].getID());
            bool destroyed = buildings[buildingIndex].getHealth() <= 0;
            recordPlayerAttackHit(targetName, x, y, damage, destroyed);

            if (buildings[buildingIndex].getHealth() <= 0) {
                std::cout << "💥 Здание #" << buildings[buildingIndex].getID() << " разрушено!" << std::endl;
                removeBuilding(buildingIndex);
                return 2;
            }
            return 0;
        }
    }

    // Атака по башне (по аналогии с HellBuild)
    if (towerIndex >= 0 && towerIndex < static_cast<int>(towers.size())) {
        int oldHealth = towers[towerIndex].getHealth();
        towers[towerIndex].takeDamage(damage);

        std::cout << "Попадание по башне #" << towers[towerIndex].getId()
                  << "! Урон: " << damage
                  << " | Было HP: " << oldHealth
                  << " | Осталось HP: " << towers[towerIndex].getHealth() << std::endl;

        const std::string targetName = "Башня #" + std::to_string(towers[towerIndex].getId());
        bool destroyed = !towers[towerIndex].isAlive() || towers[towerIndex].getHealth() <= 0;
        recordPlayerAttackHit(targetName, x, y, damage, destroyed);

        if (!towers[towerIndex].isAlive() || towers[towerIndex].getHealth() <= 0) {
            std::cout << "💥 Башня #" << towers[towerIndex].getId() << " разрушена!" << std::endl;
            // 🎯 Выдаём 3 очка прокачки за уничтожение башни
            player->AddUpgradePoints(3);
            std::cout << "⭐ +3 очка прокачки!\n";
            removeEnemyTower(towerIndex); // аналог removeBuilding
            return 3; // Код для уничтоженной башни
        }
        return 0; // Попал, но не уничтожил
    }

    return -1; // Не найдено сущности
}


// ========================================
// МЕТОДЫ ДЛЯ ИГРОКА
// ========================================

int EntityManager::playerMove(Direction dir) {
    if (dir == Direction::NONE) {
        return -1;
    }
    
    if (player->IsSlowed()) {
        std::cout << "Вы ЗАМЕДЛЕНЫ! Пропуск хода..." << std::endl;
        player->SetSlowed(false);
        moveAllEnemies();
        processBuildingSpawns();
        return 3;
    }

    auto [startX, startY] = plCoord;
    int dx = 0, dy = 0;
    directionToOffset(dir, dx, dy);
    int newX = startX + dx;
    int newY = startY + dy;

    if (newX <= 0 || newX >= gridSize - 1 || newY <= 0 || newY >= gridSize - 1) {
        return -1;
    }

    Grid& grid = board->getGrid();
    CellType targetCell = grid.getCell(newX, newY);

    if (targetCell == CellType::Wall || targetCell == CellType::HellBuild || targetCell == CellType::Tower) {
        std::cout << "Невозможно пройти через препятствие!" << std::endl;
        return 2;
    }

    // Игрок не может зайти на союзника (коллизия)
    if (targetCell == CellType::Ally) {
        std::cout << "Невозможно пройти через союзника!" << std::endl;
        return 2;
    }

    if (targetCell == CellType::Enemy) {
        if (isEnemyAt(newX, newY)) {
            int damage = getEnemyDamageAt(newX, newY);
            player->ReduceHealth(damage);

            int enemyIndex = findEnemyAtPosition(newX, newY);
            if (enemyIndex >= 0) {
                std::cout << "BONK! Враг #" << enemies[enemyIndex].GetID() << " атакует!" << std::endl;
            }
            std::cout << "Player HP:" << player->GetHealth() << std::endl;
        }
        return 1;
    }

    // Восстановление предыдущей клетки (проверяем, что там было)
    CellType oldCellType = grid.getCell(startX, startY);
    if (oldCellType == CellType::Player) {
        // Восстанавливаем то, что было до игрока
        if (isSlowTrap(startX, startY)) {
            grid.setCell(startX, startY, CellType::SlowTrap);
        } else if (findBuildingAtPosition(startX, startY) >= 0) {
            grid.setCell(startX, startY, CellType::HellBuild);
        } else if (findTowerAtPosition(startX, startY) >= 0) {
            grid.setCell(startX, startY, CellType::Tower);
        } else {
            grid.setCell(startX, startY, CellType::Empty);
        }
    }

    plCoord = {newX, newY};

    if (targetCell == CellType::SlowTrap) {
        std::cout << "⚠️  ВЫ ПОПАЛИ В ЛОВУШКУ! Следующий ход будет пропущен!" << std::endl;
        player->SetSlowed(true);
    }

    grid.setCell(newX, newY, CellType::Player);

    // Вызываем хук для установки ловушки после перемещения
    onPlayerMovedFromTo(startX, startY, newX, newY);

    moveAllEnemies();
    processBuildingSpawns();

    return 0;
}

int EntityManager::playerAttack(char key) {
    struct PlayerAttackRecorderGuard {
        EntityManager& manager;
        explicit PlayerAttackRecorderGuard(EntityManager& m) : manager(m) { manager.beginPlayerAttackRecord(); }
        ~PlayerAttackRecorderGuard() { manager.endPlayerAttackRecord(); }
    } recorder(*this);

    int distance = player->GetDistance();
    auto [startX, startY] = plCoord;

    int dirX = 0;
    int dirY = 0;

    switch (key) {
        case 'w': case 'W': dirY = -1; break;
        case 's': case 'S': dirY = 1; break;
        case 'a': case 'A': dirX = -1; break;
        case 'd': case 'D': dirX = 1; break;
        default: return -1;
    }

    Grid& grid = board->getGrid();

    for (int step = 1; step <= distance; ++step) {
        int checkX = startX + (dirX * step);
        int checkY = startY + (dirY * step);

        if (checkX <= 0 || checkX >= gridSize - 1 ||
            checkY <= 0 || checkY >= gridSize - 1) {
            std::cout << "Атака за границу поля!" << std::endl;
            return 2;
        }

        CellType cellType = grid.getCell(checkX, checkY);

        if (cellType == CellType::Wall) {
            std::cout << "Невозможно атаковать через стену!" << std::endl;
            return 2;
        }

        if (cellType == CellType::HellBuild || cellType == CellType::Enemy || cellType == CellType::Tower) {
            int damage = player->GetDamage();
            int result = handleAttackAt(checkX, checkY, damage);

            if (result >= 0) {
                return 1;
            }
        }
    }

    std::cout << "Атака в пустоту!" << std::endl;
    return 0;
}

void EntityManager::playerPosted() {
    Grid& grid = board->getGrid();
    grid.setCell(plCoord.first, plCoord.second, CellType::Player);
}

void EntityManager::playerReduceHealth(int amount) {
    player->ReduceHealth(amount);
}

int EntityManager::playerGetHealth() const {
    return player->GetHealth();
}

// ========================================
// МЕТОДЫ ДЛЯ ВРАГОВ
// ========================================

void EntityManager::addEnemy(int x, int y, int health, int damage) {
    if (x <= 0 || x >= gridSize - 1 || y <= 0 || y >= gridSize - 1) {
        std::cout << "Невозможно создать врага: некорректные координаты!" << std::endl;
        return;
    }

    Grid& grid = board->getGrid();

    if (grid.getCell(x, y) != CellType::Empty) {
        std::cout << "Невозможно создать врага: клетка занята!" << std::endl;
        return;
    }

    Enemy newEnemy(health, damage, nextEnemyID++);
    enemies.push_back(newEnemy);
    enemyCoords.push_back({x, y});
    grid.setCell(x, y, CellType::Enemy);

    std::cout << "Враг #" << newEnemy.GetID() << " создан в (" << x << ", " << y << ")" << std::endl;
}

void EntityManager::removeEnemy(int index) {
    if (index < 0 || index >= static_cast<int>(enemies.size())) return;

    auto [x, y] = enemyCoords[index];
    Grid& grid = board->getGrid();
    grid.setCell(x, y, CellType::Empty);

    enemies.erase(enemies.begin() + index);
    enemyCoords.erase(enemyCoords.begin() + index);

    std::cout << "[DEBUG] Враг удален из вектора, индекс: " << index << std::endl;
}

int EntityManager::moveEnemy(int index) {
    if (index < 0 || index >= static_cast<int>(enemies.size())) {
        std::cout << "[WARNING] moveEnemy: некорректный индекс " << index << std::endl;
        return -1;
    }
    
    if (index >= static_cast<int>(enemyCoords.size())) {
        std::cout << "[WARNING] moveEnemy: индекс выходит за границы enemyCoords" << std::endl;
        return -1;
    }
    
    if (!enemies[index].IsAlive()) return -1;

    auto [startX, startY] = enemyCoords[index];
    Grid& grid = board->getGrid();

    for (int attempt = 0; attempt < 10; ++attempt) {
        int newX = startX;
        int newY = startY;
        int key = rand() % 4;

        switch (key) {
            case 0: newX--; break;
            case 1: newX++; break;
            case 2: newY--; break;
            case 3: newY++; break;
        }

        if (newX <= 0 || newX >= gridSize - 1 || newY <= 0 || newY >= gridSize - 1) {
            continue;
        }

        CellType targetCell = grid.getCell(newX, newY);

        if (targetCell == CellType::Wall || targetCell == CellType::HellBuild || targetCell == CellType::Tower) {
            continue;
        }

        if (targetCell == CellType::Player) {
            std::cout << "Враг #" << enemies[index].GetID() << " атакует игрока!" << std::endl;
            player->ReduceHealth(enemies[index].GetDamage());
            std::cout << "Player HP:" << player->GetHealth() << std::endl;
            return 0;
        }

        // Враги не заходят на союзников (коллизия)
        if (targetCell == CellType::Ally) {
            int allyIndex = findAllyAtPosition(newX, newY);
            if (allyIndex >= 0 && allyIndex < static_cast<int>(allies.size())) {
                // Враг атакует союзника
                int enemyDamage = enemies[index].GetDamage();
                allies[allyIndex].ReduceHealth(enemyDamage);
                
                std::cout << "💥 Враг #" << enemies[index].GetID() 
                          << " атакует союзника #" << allies[allyIndex].GetID()
                          << "! Урон: " << enemyDamage << std::endl;
                std::cout << "Союзник #" << allies[allyIndex].GetID() 
                          << " HP:" << allies[allyIndex].GetHealth() << std::endl;

                // Если союзник умер - удаляем его
                if (!allies[allyIndex].IsAlive()) {
                    std::cout << "💀 Союзник #" << allies[allyIndex].GetID() << " повержен врагом!" << std::endl;
                    removeAlly(allyIndex);
                }
            }
            return 0; // Враг не перемещается на клетку союзника
        }

        if (targetCell == CellType::Enemy) {
            continue;
        }

        // Проверка на ловушку игрока перед перемещением через TrapRegistry
        int trapDmg = TrapRegistry::triggerAt(newX, newY);
        if (trapDmg > 0) {
            // Враг наступил на ловушку игрока - наносим урон
            enemies[index].ReduceHealth(trapDmg);
            std::cout << "💥 Враг #" << enemies[index].GetID()
                      << " попал на ловушку! Урон: " << trapDmg << std::endl;

            // Если враг умер - удаляем его и не перемещаем
            if (!enemies[index].IsAlive()) {
                std::cout << "💀 Враг #" << enemies[index].GetID() << " повержен ловушкой!" << std::endl;
                // Очистить старую клетку врага (восстанавливаем то, что было)
                if (isSlowTrap(startX, startY)) {
                    grid.setCell(startX, startY, CellType::SlowTrap);
                } else if (findBuildingAtPosition(startX, startY) >= 0) {
                    grid.setCell(startX, startY, CellType::HellBuild);
                } else if (findTowerAtPosition(startX, startY) >= 0) {
                    grid.setCell(startX, startY, CellType::Tower);
                } else {
                    grid.setCell(startX, startY, CellType::Empty);
                }
                removeEnemy(index);
                return 0; // Враг удален, клетка восстановлена
            }
            // Если выжил - продолжаем перемещение
        }

        // Восстановление предыдущей клетки (проверяем, что там было)
        CellType oldCellType = grid.getCell(startX, startY);
        if (oldCellType == CellType::Enemy) {
            // Восстанавливаем то, что было до врага
            if (isSlowTrap(startX, startY)) {
                grid.setCell(startX, startY, CellType::SlowTrap);
            } else if (findBuildingAtPosition(startX, startY) >= 0) {
                grid.setCell(startX, startY, CellType::HellBuild);
            } else if (findTowerAtPosition(startX, startY) >= 0) {
                grid.setCell(startX, startY, CellType::Tower);
            } else {
                grid.setCell(startX, startY, CellType::Empty);
            }
        }

        // Обновление позиции
        enemyCoords[index] = {newX, newY};
        grid.setCell(newX, newY, CellType::Enemy);
        return 0;
    }

    return -1;
}

void EntityManager::moveAllEnemies() {
    for (size_t i = 0; i < enemies.size(); ++i) {
        moveEnemy(static_cast<int>(i));
    }
}

int EntityManager::findEnemyAtPosition(int x, int y) const {
    for (size_t i = 0; i < enemyCoords.size(); ++i) {
        if (enemyCoords[i].first == x && enemyCoords[i].second == y) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int EntityManager::getEnemyCount() const noexcept {
    return enemies.size();
}

const std::vector<Enemy>& EntityManager::getEnemies() const {
    return enemies;
}

const std::vector<std::pair<int,int>>& EntityManager::getEnemyCoords() const {
    return enemyCoords;
}

bool EntityManager::isEnemyAt(int x, int y) const {
    return findEnemyAtPosition(x, y) >= 0;
}

int EntityManager::getEnemyDamageAt(int x, int y) const {
    int index = findEnemyAtPosition(x, y);
    if (index >= 0) {
        return enemies[index].GetDamage();
    }
    return 0;
}


// ========================================
// МЕТОДЫ ДЛЯ ЗДАНИЙ
// ========================================

void EntityManager::addEnemyBuilding(int x, int y, int spawnInterval) {
    if (x <= 0 || x >= gridSize - 1 || y <= 0 || y >= gridSize - 1) {
        std::cout << "Невозможно разместить здание: некорректные координаты!" << std::endl;
        return;
    }

    Grid& grid = board->getGrid();

    CellType currentCell = grid.getCell(x, y);
    if (currentCell != CellType::Empty && currentCell != CellType::Player) {
        std::cout << "Невозможно разместить здание: клетка занята! (cellType=" 
                  << static_cast<int>(currentCell) << ")" << std::endl;
        return;
    }

    EnemyBuilding newBuilding(x, y, spawnInterval, nextBuildingID++);
    buildings.push_back(newBuilding);
    grid.setCell(x, y, CellType::HellBuild);

    std::cout << "[DEBUG] Вражеское здание #" << newBuilding.getID() << " создано в ("
              << x << ", " << y << ") с интервалом спавна " << spawnInterval << " ходов" << std::endl;
    std::cout << "[DEBUG] Grid установлен: getCell(" << x << ", " << y << ")=" 
              << static_cast<int>(grid.getCell(x, y)) << " (должно быть HellBuild=" 
              << static_cast<int>(CellType::HellBuild) << ")" << std::endl;
}

int EntityManager::spawnEnemyNearBuilding(int buildingX, int buildingY) {
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    Grid& grid = board->getGrid();

    for (int i = 0; i < 4; i++) {
        int j = rand() % 4;
        std::swap(directions[i][0], directions[j][0]);
        std::swap(directions[i][1], directions[j][1]);
    }

    for (int i = 0; i < 4; i++) {
        int newX = buildingX + directions[i][1];
        int newY = buildingY + directions[i][0];

        if (newX <= 0 || newX >= gridSize - 1 || newY <= 0 || newY >= gridSize - 1) {
            continue;
        }

        if (grid.getCell(newX, newY) == CellType::Empty) {
            addEnemy(newX, newY, 2, 1);
            return 0;
        }
    }

    return -1;
}

void EntityManager::processBuildingSpawns() {
    for (auto& building : buildings) {
        if (!building.isActive() || building.getHealth() <= 0) {
            continue;
        }

        if (building.shouldSpawn()) {
            int result = spawnEnemyNearBuilding(building.getX(), building.getY());

            if (result == 0) {
                std::cout << "Здание #" << building.getID() << " создало нового врага!" << std::endl;
            } else {
                std::cout << "Здание #" << building.getID() << " не смогло создать врага (нет места)" << std::endl;
            }
            building.resetTimer();
        }
    }
}

// Добавим метод удаления здания (как с врагами)
void EntityManager::removeBuilding(int index) {
    if (index < 0 || index >= static_cast<int>(buildings.size())) {
        std::cout << "[WARNING] removeBuilding: некорректный индекс " << index << std::endl;
        return;
    }

    auto [x, y] = std::make_pair(buildings[index].getX(), buildings[index].getY());
    Grid& grid = board->getGrid();
    grid.setCell(x, y, CellType::Empty);

    buildings.erase(buildings.begin() + index);

    std::cout << "[DEBUG] Здание удалено из вектора, индекс: " << index << std::endl;
}

// Метод для проверки количества АКТИВНЫХ зданий
int EntityManager::getActiveBuildingCount() const noexcept {
    int count = 0;
    for (const auto& building : buildings) {
        if (building.isActive() && building.getHealth() > 0) {
            count++;
        }
    }
    return count;
}

int EntityManager::getAliveEnemyCount() const noexcept {
    int count = 0;
    for (const auto& enemy : enemies) {
        if (enemy.IsAlive()) {
            count++;
        }
    }
    return count;
}

int EntityManager::findBuildingAtPosition(int x, int y) const {
    for (size_t i = 0; i < buildings.size(); ++i) {
        if (buildings[i].getX() == x && buildings[i].getY() == y) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int EntityManager::getBuildingCount() const noexcept {
    return buildings.size();
}

const std::vector<EnemyBuilding>& EntityManager::getBuildings() const {
    return buildings;
}

// === МЕТОДЫ ДЛЯ БАШЕН ===

void EntityManager::addEnemyTower(int x, int y, int health) {
    towers.emplace_back(x, y, health);
    std::cout << "🏯 Враждебная башня добавлена на (" << x << ", " << y << ")\n";
}

void EntityManager::removeEnemyTower(int index) {
    if (index < 0 || index >= static_cast<int>(towers.size())) {
        std::cout << "[WARNING] removeEnemyTower: некорректный индекс " << index << std::endl;
        return;
    }

    auto [x, y] = std::make_pair(towers[index].getX(), towers[index].getY());
    Grid& grid = board->getGrid();
    grid.setCell(x, y, CellType::Empty);

    towers.erase(towers.begin() + index);
    std::cout << "[DEBUG] Башня удалена из вектора, индекс: " << index << std::endl;
}

const std::vector<EnemyTower>& EntityManager::getTowers() const {
    return towers;
}

int EntityManager::getTowerCount() const noexcept {
    return towers.size();
}

void EntityManager::updateAllTowers() {
    // Обновляем кулдаун всех башен
    for (auto& tower : towers) {
        tower.tickCooldown();
    }
}

int EntityManager::checkTowerAttack(int playerX, int playerY) {
    // Теперь башни атакуют через spell->use в tryAttack
    // Этот метод вызывается в конце хода игрока
    for (auto& tower : towers) {
        if (tower.isAlive()) {
            tower.tryAttack(*this, *board, gridSize, playerX, playerY);
        }
    }
    return 0;  // Урон наносится напрямую через spell->use, не возвращаем значение
}

void EntityManager::armPlayerTrapAfterMove() {
    pendingTrapArm = true;
}

void EntityManager::placePlayerTrapAt(int x, int y) {
    // Проверка границ
    if (x <= 0 || x >= gridSize - 1 || y <= 0 || y >= gridSize - 1) {
        std::cout << "Невозможно разместить ловушку: координаты выходят за границы поля!" << std::endl;
        return;
    }

    Grid& grid = board->getGrid();
    CellType cell = grid.getCell(x, y);

    // Проверка на занятые клетки
    if (cell != CellType::Empty && cell != CellType::SlowTrap) {
        std::cout << "Невозможно разместить ловушку: клетка занята!" << std::endl;
        return;
    }

    // Проверка на наличие других ловушек
    if (isSlowTrap(x, y)) {
        std::cout << "Невозможно разместить ловушку: на этой клетке уже есть замедляющая ловушка!" << std::endl;
        return;
    }

    if (isPlayerTrapAt(x, y)) {
        std::cout << "Невозможно разместить ловушку: на этой клетке уже есть ловушка игрока!" << std::endl;
        return;
    }

    // Размещаем ловушку
    playerTraps.emplace_back(x, y);
    std::cout << "✓ Ловушка игрока размещена на (" << x << ", " << y << ")" << std::endl;
}

bool EntityManager::isPlayerTrapAt(int x, int y) const {
    for (const auto& pt : playerTraps) {
        if (pt.first == x && pt.second == y) return true;
    }
    return false;
}

void EntityManager::removePlayerTrapAt(int x, int y) {
    for (size_t i = 0; i < playerTraps.size(); ++i) {
        if (playerTraps[i].first == x && playerTraps[i].second == y) {
            playerTraps.erase(playerTraps.begin() + static_cast<long>(i));
            break;
        }
    }
}

const std::vector<std::pair<int,int>>& EntityManager::getPlayerTraps() const {
    return playerTraps;
}

void EntityManager::onPlayerMovedFromTo(int oldX, int oldY, int newX, int newY) {
    if (pendingTrapArm && (oldX != newX || oldY != newY)) {
        placePlayerTrapAt(oldX, oldY);
        pendingTrapArm = false;
    }
}



int EntityManager::findTowerAtPosition(int x, int y) const {
    for (int i = 0; i < static_cast<int>(towers.size()); ++i) {
        if (towers[i].getX() == x && towers[i].getY() == y) return i;
    }
    return -1;
}


// ========================================
// ЛОВУШКИ
// ========================================

void EntityManager::addSlowTrap(int x, int y) {
    slowTraps.push_back({x, y});
}

void EntityManager::setSlowTraps(const std::vector<std::pair<int,int>>& traps) {
    slowTraps = traps;
}

bool EntityManager::isSlowTrap(int x, int y) const {
    for (const auto& trap : slowTraps) {
        if (trap.first == x && trap.second == y) {
            return true;
        }
    }
    return false;
}

void EntityManager::checkEnemyTraps() {
    // Для каждого врага: если встал на PlayerTrap — получает урон и ловушка исчезает
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (!enemies[i].IsAlive()) continue;
        int ex = enemyCoords[i].first;
        int ey = enemyCoords[i].second;

        if (isPlayerTrapAt(ex, ey)) {
            enemies[i].ReduceHealth(playerTrapDamage);
            std::cout << "💥 Враг #" << enemies[i].GetID()
                      << " попал на ловушку игрока! Урон: " << playerTrapDamage << "\n";
            removePlayerTrapAt(ex, ey);

            if (!enemies[i].IsAlive()) {
                std::cout << "💀 Враг #" << enemies[i].GetID() << " повержен ловушкой!\n";
                removeEnemy(static_cast<int>(i));
                // осторожно: removeEnemy меняет векторы; но мы выходим/продолжаем корректно,
                // можно сделать i--, но в вашем пайплайне движение по индексу безопасно,
                // если сразу break; хотите — оставьте без удаления, а пометьте смертью и удалите позже
                // Для простоты здесь продолжаем без i--
            }
        }
    }
}

int EntityManager::checkTrapDamage(int x, int y) {
    // SlowTrap влияет ТОЛЬКО на игрока (как было)
    // PlayerTrap — только на врагов; потому здесь возвращаем 0
    (void)x; (void)y;
    return 0;
}

// ========================================
// МЕТОДЫ ДЛЯ СОЮЗНИКОВ
// ========================================

void EntityManager::addAlly(int x, int y, int health, int damage) {
    if (x <= 0 || x >= gridSize - 1 || y <= 0 || y >= gridSize - 1) {
        std::cout << "Невозможно создать союзника: некорректные координаты!" << std::endl;
        return;
    }

    Grid& grid = board->getGrid();

    if (grid.getCell(x, y) != CellType::Empty) {
        std::cout << "Невозможно создать союзника: клетка занята!" << std::endl;
        return;
    }

    Ally newAlly(health, damage, nextAllyID++);
    allies.push_back(newAlly);
    allyCoords.push_back({x, y});
    grid.setCell(x, y, CellType::Ally);

    std::cout << "Союзник #" << newAlly.GetID() << " создан в (" << x << ", " << y << ")" << std::endl;
}

void EntityManager::removeAlly(int index) {
    if (index < 0 || index >= static_cast<int>(allies.size())) {
        std::cout << "[WARNING] removeAlly: некорректный индекс " << index << std::endl;
        return;
    }
    
    if (index >= static_cast<int>(allyCoords.size())) {
        std::cout << "[WARNING] removeAlly: индекс выходит за границы allyCoords" << std::endl;
        return;
    }

    auto [x, y] = allyCoords[index];
    Grid& grid = board->getGrid();
    
    // Восстанавливаем то, что было до союзника
    if (isSlowTrap(x, y)) {
        grid.setCell(x, y, CellType::SlowTrap);
    } else if (findBuildingAtPosition(x, y) >= 0) {
        grid.setCell(x, y, CellType::HellBuild);
    } else if (findTowerAtPosition(x, y) >= 0) {
        grid.setCell(x, y, CellType::Tower);
    } else {
        grid.setCell(x, y, CellType::Empty);
    }

    allies.erase(allies.begin() + index);
    allyCoords.erase(allyCoords.begin() + index);

    std::cout << "[DEBUG] Союзник удален из вектора, индекс: " << index << std::endl;
}

int EntityManager::moveAlly(int index) {
    if (index < 0 || index >= static_cast<int>(allies.size())) {
        return -1;
    }
    
    if (index >= static_cast<int>(allyCoords.size())) {
        return -1;
    }
    
    if (!allies[index].IsAlive()) return -1;

    auto [startX, startY] = allyCoords[index];
    Grid& grid = board->getGrid();

    // Союзник всегда пытается переместиться (как игрок) - пробуем все 4 направления
    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};  // вверх, вниз, влево, вправо
    
    // Перемешиваем направления для случайности
    for (int i = 0; i < 4; ++i) {
        int j = rand() % 4;
        std::swap(directions[i][0], directions[j][0]);
        std::swap(directions[i][1], directions[j][1]);
    }

    for (int attempt = 0; attempt < 4; ++attempt) {
        int newX = startX + directions[attempt][0];
        int newY = startY + directions[attempt][1];

        // Проверка границ
        if (newX <= 0 || newX >= gridSize - 1 || newY <= 0 || newY >= gridSize - 1) {
            continue;
        }

        CellType targetCell = grid.getCell(newX, newY);

        // Союзники не заходят на препятствия (стены, здания, башни)
        if (targetCell == CellType::Wall || targetCell == CellType::HellBuild || 
            targetCell == CellType::Tower) {
            continue;
        }

        // Союзники не заходят на игрока и других союзников
        if (targetCell == CellType::Player || targetCell == CellType::Ally) {
            continue;
        }

        // Восстановление предыдущей клетки
        CellType oldCellType = grid.getCell(startX, startY);
        if (oldCellType == CellType::Ally) {
            if (isSlowTrap(startX, startY)) {
                grid.setCell(startX, startY, CellType::SlowTrap);
            } else if (findBuildingAtPosition(startX, startY) >= 0) {
                grid.setCell(startX, startY, CellType::HellBuild);
            } else if (findTowerAtPosition(startX, startY) >= 0) {
                grid.setCell(startX, startY, CellType::Tower);
            } else {
                grid.setCell(startX, startY, CellType::Empty);
            }
        }

        // Если союзник заходит на клетку с врагом - получает урон (как игрок)
        if (targetCell == CellType::Enemy) {
            int enemyIndex = findEnemyAtPosition(newX, newY);
            if (enemyIndex >= 0 && enemyIndex < static_cast<int>(enemies.size())) {
                int enemyDamage = enemies[enemyIndex].GetDamage();
                allies[index].ReduceHealth(enemyDamage);
                
                std::cout << "💥 Союзник #" << allies[index].GetID() 
                          << " столкнулся с врагом #" << enemies[enemyIndex].GetID()
                          << "! Получил " << enemyDamage << " урона. HP: " 
                          << allies[index].GetHealth() << std::endl;

                // Если союзник умер - удаляем его и не перемещаем
                if (!allies[index].IsAlive()) {
                    std::cout << "💀 Союзник #" << allies[index].GetID() << " повержен!" << std::endl;
                    removeAlly(index);
                    return 0; // Союзник удален
                }
                // Союзник выжил - не перемещаем его на клетку врага (как игрок)
                return 0;
            }
            continue; // Враг не найден - пробуем другую клетку
        }

        // Обновление позиции (пустая клетка или ловушка)
        allyCoords[index] = {newX, newY};
        grid.setCell(newX, newY, CellType::Ally);
        
        return 0; // Успешное перемещение
    }

    // Не удалось найти подходящую клетку - союзник остается на месте
    return 0;
}

void EntityManager::moveAllies() {
    // Проходим по союзникам с конца, чтобы избежать проблем с индексами при удалении
    for (int i = static_cast<int>(allies.size()) - 1; i >= 0; --i) {
        if (i < static_cast<int>(allies.size()) && allies[i].IsAlive()) {
            moveAlly(i);
            // После moveAlly союзник может быть удален, поэтому проверяем размер
        }
    }
}

int EntityManager::findAllyAtPosition(int x, int y) const {
    for (size_t i = 0; i < allyCoords.size(); ++i) {
        if (allyCoords[i].first == x && allyCoords[i].second == y) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int EntityManager::getAllyCount() const noexcept {
    return allies.size();
}

const std::vector<Ally>& EntityManager::getAllies() const {
    return allies;
}

const std::vector<std::pair<int,int>>& EntityManager::getAllyCoords() const {
    return allyCoords;
}

bool EntityManager::isAllyAt(int x, int y) const {
    return findAllyAtPosition(x, y) >= 0;
}

std::pair<int, int> EntityManager::findFreeAdjacentCell(int centerX, int centerY) const {
    Grid& grid = board->getGrid();
    
    // Проверяем 4 стороны (верх, низ, лево, право)
    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    
    for (int i = 0; i < 4; ++i) {
        int newX = centerX + directions[i][0];
        int newY = centerY + directions[i][1];
        
        // Проверка границ
        if (newX <= 0 || newX >= gridSize - 1 || newY <= 0 || newY >= gridSize - 1) {
            continue;
        }
        
        // Проверка, что клетка пустая
        CellType cell = grid.getCell(newX, newY);
        if (cell == CellType::Empty) {
            return {newX, newY};
        }
    }
    
    // Если не нашли в 4 сторонах, проверяем диагонали (8 направлений)
    int diagonalDirs[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    for (int i = 0; i < 4; ++i) {
        int newX = centerX + diagonalDirs[i][0];
        int newY = centerY + diagonalDirs[i][1];
        
        if (newX <= 0 || newX >= gridSize - 1 || newY <= 0 || newY >= gridSize - 1) {
            continue;
        }
        
        CellType cell = grid.getCell(newX, newY);
        if (cell == CellType::Empty) {
            return {newX, newY};
        }
    }
    
    // Не найдено свободной клетки
    return {-1, -1};
}


