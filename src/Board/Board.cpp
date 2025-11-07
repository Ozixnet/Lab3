#include "Board/Board.h"
#include "Entity/Enemies/EnemyTower.h"
#include "Magic/TrapRegistry.h"
#include <iostream>
#include <cmath>

Board::Board(int s, Player& p)
     :size(s),
      player(p),
      grid(s, CellType::Empty),
      entityManager(this, size, &p) {

    if (size < 10 || size > 25) {
        throw std::invalid_argument("Board size must be in [10,25]");
    }

    // Инициализация стен
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (i == 0 || j == 0 || i == size - 1 || j == size - 1) {
                grid.setCell(j, i, CellType::Wall);
            }
        }
    }

    std::cout << "Board created" << std::endl;
}

// === КОНСТРУКТОРЫ КОПИРОВАНИЯ/ПЕРЕМЕЩЕНИЯ ===
Board::Board(const Board& other)
    : size(other.size), player(other.player), grid(other.grid),
      entityManager(other.entityManager), slowTraps(other.slowTraps) {}

Board::Board(Board&& other) noexcept
    : size(other.size), player(other.player), grid(std::move(other.grid)),
      entityManager(std::move(other.entityManager)), slowTraps(std::move(other.slowTraps)) {}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        size = other.size;
        grid = other.grid;
        entityManager = other.entityManager;
        slowTraps = other.slowTraps;
    }
    return *this;
}

Board& Board::operator=(Board&& other) noexcept {
    if (this != &other) {
        size = other.size;
        grid = std::move(other.grid);
        entityManager = std::move(other.entityManager);
        slowTraps = std::move(other.slowTraps);
    }
    return *this;
}

// === ГЛАВНЫЙ МЕТОД ОТОБРАЖЕНИЯ ===
void Board::displayBoard() const {
    // ← СОЗДАЕМ ВРЕМЕННУЮ КОПИЮ для отображения (реальная сетка не меняется!)
    Grid displayGrid = grid;

    // Очищаем копию (кроме стен)
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            CellType current = displayGrid.getCell(j, i);
            if (current != CellType::Wall) {
                displayGrid.setCell(j, i, CellType::Empty);
            }
        }
    }

    // === 1. ОТОБРАЖЕНИЕ ДИАПАЗОНОВ БАШЕН (нижний слой) ===
    // Показываем радиус только если башня готова атаковать (cooldownCur == 0)
    const auto& towers = entityManager.getTowers();
    for (const auto& tower : towers) {
        if (!tower.isAlive()) continue;
        
        // Показываем радиус только если башня готова атаковать
        if (!tower.canAttackNow()) continue;
        
        int tx = tower.getX();
        int ty = tower.getY();
        int radius = tower.getAttackRadius();

        // Отобразить диапазон атаки (Chebyshev distance)
        for (int x = std::max(0, tx - radius); x <= std::min(size - 1, tx + radius); ++x) {
            for (int y = std::max(0, ty - radius); y <= std::min(size - 1, ty + radius); ++y) {
                if (x < 0 || x >= size || y < 0 || y >= size) continue;  // Дополнительная проверка границ
                
                int distX = std::abs(x - tx);
                int distY = std::abs(y - ty);

                if (std::max(distX, distY) <= radius && displayGrid.getCell(x, y) == CellType::Empty) {
                    displayGrid.setCell(x, y, CellType::TowerRange);
                }
            }
        }
    }

    // === 2. ОТОБРАЖЕНИЕ ЛОВУШЕК ИГРОКА ===
    const auto& traps = TrapRegistry::getAll();
    for (const auto& trap : traps) {
        if (!trap.active) continue;
        
        int tx = trap.x;
        int ty = trap.y;

        if (tx >= 0 && tx < size && ty >= 0 && ty < size) {
            if (displayGrid.getCell(tx, ty) == CellType::Empty) {
                displayGrid.setCell(tx, ty, CellType::PlayerTrap);
            }
        }
    }

    // === 3. ОТОБРАЖЕНИЕ ВРАГОВ ===
    const auto& enemies = entityManager.getEnemies();
    const auto& enemyCoords = entityManager.getEnemyCoords();
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (i < enemyCoords.size() && enemies[i].IsAlive()) {
            int ex = enemyCoords[i].first;
            int ey = enemyCoords[i].second;

            if (ex >= 0 && ex < size && ey >= 0 && ey < size) {
                displayGrid.setCell(ex, ey, CellType::Enemy);
            }
        }
    }

    // === 4. ОТОБРАЖЕНИЕ ЗДАНИЙ ===
    const auto& buildings = entityManager.getBuildings();
    for (const auto& building : buildings) {
        if (building.getHealth() > 0) {
            int bx = building.getX();
            int by = building.getY();

            if (bx >= 0 && bx < size && by >= 0 && by < size) {
                displayGrid.setCell(bx, by, CellType::HellBuild);
            }
        }
    }

    // === 5. ОТОБРАЖЕНИЕ БАШЕН (поверх диапазона) ===
    for (const auto& tower : towers) {
        if (tower.isAlive()) {
            int tx = tower.getX();
            int ty = tower.getY();

            if (tx >= 0 && tx < size && ty >= 0 && ty < size) {
                displayGrid.setCell(tx, ty, CellType::Tower);
            }
        }
    }

    // === 5.5. ОТОБРАЖЕНИЕ СОЮЗНИКОВ ===
    const auto& allies = entityManager.getAllies();
    const auto& allyCoords = entityManager.getAllyCoords();
    for (size_t i = 0; i < allies.size(); ++i) {
        if (i < allyCoords.size() && allies[i].IsAlive()) {
            int ax = allyCoords[i].first;
            int ay = allyCoords[i].second;

            if (ax >= 0 && ax < size && ay >= 0 && ay < size) {
                displayGrid.setCell(ax, ay, CellType::Ally);
            }
        }
    }

    // === 6. ОТОБРАЖЕНИЕ ИГРОКА (самый верхний слой) ===
    auto [px, py] = entityManager.getPlayerCoord();
    if (px >= 0 && px < size && py >= 0 && py < size) {
        displayGrid.setCell(px, py, CellType::Player);
    }

    // === ОТРИСОВКА КОПИИ (оригинальная сетка не изменена!) ===
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            std::cout << glyph(displayGrid.getCell(j, i)) << " ";
        }
        std::cout << '\n';
    }

    // Показываем только ЖИВЫХ врагов
    int aliveEnemies = entityManager.getAliveEnemyCount();
    std::cout << "\n=== Враги на карте (" << aliveEnemies << ") ===" << std::endl;

    if (aliveEnemies > 0) {
        const auto& enemies = entityManager.getEnemies();
        const auto& enemyCoords = entityManager.getEnemyCoords();
        for (size_t i = 0; i < enemies.size(); ++i) {
            if (enemies[i].IsAlive()) {
                std::cout << "  👹 Enemy #" << enemies[i].GetID()
                          << " [" << enemyCoords[i].first << "," << enemyCoords[i].second << "]"
                          << " HP:" << enemies[i].GetHealth() << std::endl;
            }
        }
    } else {
        std::cout << "  Все враги повержены!" << std::endl;
    }

    // Показываем только АКТИВНЫЕ здания
    int activeBuildings = entityManager.getActiveBuildingCount();
    std::cout << "\n=== Вражеские здания (" << activeBuildings << ") ===" << std::endl;

    if (activeBuildings > 0) {
        const auto& buildings = entityManager.getBuildings();
        for (const auto& building : buildings) {
            if (building.isActive() && building.getHealth() > 0) {
                std::cout << "  🏰 Building #" << building.getID()
                          << " [" << building.getX() << "," << building.getY() << "]"
                          << " HP:" << building.getHealth()
                          << " Timer:" << building.getCurrentTimer() << "/" << building.getSpawnInterval() << "\n" << std::endl;
            }
        }
    } else {
        std::cout << "  Все здания разрушены!" << std::endl;
    }

    // === ОТОБРАЖЕНИЕ СОЮЗНИКОВ ===
    int aliveAllies = 0;
    for (size_t i = 0; i < allies.size(); ++i) {
        if (i < allyCoords.size() && allies[i].IsAlive()) {
            aliveAllies++;
        }
    }

    std::cout << "\n=== Союзники (" << aliveAllies << ") ===" << std::endl;

    if (aliveAllies > 0) {
        for (size_t i = 0; i < allies.size(); ++i) {
            if (i < allyCoords.size() && allies[i].IsAlive()) {
                std::cout << "  ▲ Ally #" << allies[i].GetID()
                          << " [" << allyCoords[i].first << "," << allyCoords[i].second << "]"
                          << " HP:" << allies[i].GetHealth() << std::endl;
            }
        }
    } else {
        std::cout << "  Нет союзников на поле" << std::endl;
    }

    // === ОТОБРАЖЕНИЕ БАШЕН ===
    int aliveTowers = 0;
    for (const auto& tower : towers) {
        if (tower.isAlive()) aliveTowers++;
    }

    std::cout << "\n=== Вражеские башни (" << aliveTowers << ") ===" << std::endl;

    if (aliveTowers > 0) {
        for (const auto& tower : towers) {
            if (tower.isAlive()) {
                std::cout << "  🏯 Tower #" << tower.getId()
                          << " [" << tower.getX() << "," << tower.getY() << "]"
                          << " HP:" << tower.getHealth() << "/" << tower.getMaxHealth()
                          << " Cooldown:" << tower.getCooldown() << "\n" << std::endl;
            }
        }
    } else {
        std::cout << "  Все башни разрушены!" << std::endl;
    }

    // === ОТОБРАЖЕНИЕ ЛОВУШЕК ИГРОКА ===
    int activeTrapCount = 0;
    for (const auto& trap : traps) {
        if (trap.active) activeTrapCount++;
    }
    
    std::cout << "\n=== Размещённые ловушки (" << activeTrapCount << ") ===" << std::endl;

    if (activeTrapCount > 0) {
        for (const auto& trap : traps) {
            if (trap.active) {
                std::cout << "  ● Trap [" << trap.x << "," << trap.y << "]"
                          << " Урон:" << trap.damage << "\n" << std::endl;
            }
        }
    } else {
        std::cout << "  Ловушек не размещено" << std::endl;
    }
}

void Board::addSlowTraps(int count) {
    auto playerCoord = entityManager.getPlayerCoord();

    for (int i = 0; i < count; ++i) {
        while (true) {
            int x = 1 + (rand() % (size - 2));
            int y = 1 + (rand() % (size - 2));

            if (grid.getCell(x, y) == CellType::Empty &&
                !(x == playerCoord.first && y == playerCoord.second) &&
                !(abs(x - playerCoord.first) <= 1 && abs(y - playerCoord.second) <= 1)) {

                grid.setCell(x, y, CellType::SlowTrap);
                slowTraps.push_back({x, y});
                entityManager.addSlowTrap(x, y);
                break;
            }
        }
    }
    std::cout << "Создано " << count << " замедляющих ловушек (~)" << std::endl;
}
