#include "Magic/GameSpellContext.h"
#include "Entity/EntityManager.h"
#include "Board/Board.h"
#include "Board/Grid.h"
#include "Board/Cell.h"
#include "Magic/TrapRegistry.h"
#include "Magic/SpellBuffContext.h"
#include <iostream>
#include <cmath>
#include <algorithm>

GameSpellContext::GameSpellContext(EntityManager& em, Board& b, int gs)
    : entityManager(em), board(b), gridSize(gs) {}

std::pair<int, int> GameSpellContext::getPlayerPosition() const {
    return entityManager.getPlayerCoord();
}

int GameSpellContext::getGridSize() const {
    return gridSize;
}

std::vector<TargetInfo> GameSpellContext::findTargetsInRadius(int centerX, int centerY, int radius) const {
    std::vector<TargetInfo> targets;
    
    // Получаем всех врагов
    const auto& enemies = entityManager.getEnemies();
    const auto& enemyCoords = entityManager.getEnemyCoords();
    
    for (size_t idx = 0; idx < enemies.size(); ++idx) {
        if (!enemies[idx].IsAlive()) continue;
        
        if (idx < enemyCoords.size()) {
            auto [ex, ey] = enemyCoords[idx];
            int distX = std::abs(ex - centerX);
            int distY = std::abs(ey - centerY);
            
            // Проверяем квадратный радиус
            if (distX <= radius && distY <= radius) {
                TargetInfo target;
                target.x = ex;
                target.y = ey;
                target.type = TargetInfo::Type::Enemy;
                target.index = static_cast<int>(idx);
                target.health = enemies[idx].GetHealth();
                target.displayName = "Enemy #" + std::to_string(idx);
                targets.push_back(target);
            }
        }
    }
    
    // Получаем все здания
    const auto& buildings = entityManager.getBuildings();
    for (size_t idx = 0; idx < buildings.size(); ++idx) {
        const auto& building = buildings[idx];
        if (!building.isActive() || building.getHealth() <= 0) continue;
        
        int bx = building.getX();
        int by = building.getY();
        int distX = std::abs(bx - centerX);
        int distY = std::abs(by - centerY);
        
        if (distX <= radius && distY <= radius) {
            TargetInfo target;
            target.x = bx;
            target.y = by;
            target.type = TargetInfo::Type::Building;
            target.index = static_cast<int>(idx);
            target.health = building.getHealth();
            target.displayName = "Building #" + std::to_string(idx);
            targets.push_back(target);
        }
    }
    
    // Получаем все башни
    const auto& towers = entityManager.getTowers();
    for (size_t idx = 0; idx < towers.size(); ++idx) {
        const auto& tower = towers[idx];
        if (tower.getHealth() <= 0) continue;
        
        int tx = tower.getX();
        int ty = tower.getY();
        int distX = std::abs(tx - centerX);
        int distY = std::abs(ty - centerY);
        
        if (distX <= radius && distY <= radius) {
            TargetInfo target;
            target.x = tx;
            target.y = ty;
            target.type = TargetInfo::Type::Tower;
            target.index = static_cast<int>(idx);
            target.health = tower.getHealth();
            target.displayName = "Tower #" + std::to_string(idx);
            targets.push_back(target);
        }
    }
    
    return targets;
}

bool GameSpellContext::isCellEmpty(int x, int y) const {
    if (x < 0 || x >= gridSize || y < 0 || y >= gridSize) {
        return false;
    }
    
    Grid& grid = board.getGrid();
    CellType cell = grid.getCell(x, y);
    
    // Клетка считается пустой, если это Empty или Player
    return cell == CellType::Empty || cell == CellType::Player;
}

bool GameSpellContext::hasTrapAt(int x, int y) const {
    // Проверяем, есть ли ловушка в TrapRegistry
    return TrapRegistry::isTrapAt(x, y);
}

bool GameSpellContext::dealDamage(int x, int y, int damage) {
    int result = entityManager.handleAttackAt(x, y, damage);
    return result > 0;
}

bool GameSpellContext::dealDamageToTarget(const TargetInfo& target, int damage) {
    return dealDamage(target.x, target.y, damage);
}

bool GameSpellContext::placeTrap(int x, int y, int damage) {
    // Проверяем границы
    if (x < 0 || x >= gridSize || y < 0 || y >= gridSize) {
        return false;
    }
    
    // Проверяем, что клетка подходящая
    Grid& grid = board.getGrid();
    CellType cell = grid.getCell(x, y);
    
    // Нельзя ставить на занятые клетки
    if (cell != CellType::Empty && cell != CellType::Player && cell != CellType::SlowTrap) {
        return false;
    }
    
    // Нельзя перекрывать ловушку поля
    if (cell == CellType::SlowTrap) {
        return false;
    }
    
    // Проверяем, что тут нет игровой ловушки
    if (TrapRegistry::isTrapAt(x, y)) {
        return false;
    }
    
    // Размещаем ловушку
    return TrapRegistry::addTrap(x, y, damage);
}

bool GameSpellContext::summonAlly(int x, int y, int health, int damage) {
    if (x < 0 || x >= gridSize || y < 0 || y >= gridSize) {
        return false;
    }
    
    if (!isCellEmpty(x, y)) {
        return false;
    }
    
    entityManager.addAlly(x, y, health, damage);
    return true;
}

std::pair<int, int> GameSpellContext::findFreeAdjacentCell(int centerX, int centerY) const {
    return entityManager.findFreeAdjacentCell(centerX, centerY);
}

SpellBuffContext& GameSpellContext::getBuffContext() {
    return entityManager.getBuffContext();
}

const SpellBuffContext& GameSpellContext::getBuffContext() const {
    return entityManager.getBuffContext();
}

int GameSpellContext::requestTargetSelection(const std::vector<TargetInfo>& targets) const {
    if (targets.empty()) {
        return -1;
    }
    
    std::cout << "\n=== Доступные цели ===" << std::endl;
    for (size_t i = 0; i < targets.size(); ++i) {
        const auto& target = targets[i];
        std::cout << "[" << i << "] " << target.displayName 
                  << " на (" << target.x << ", " << target.y << ")"
                  << " HP: " << target.health << std::endl;
    }
    
    std::cout << "\nВыберите цель (0-" << (targets.size() - 1) 
              << ") или -1 для отмены: ";
    
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return -1;
    }
    
    if (choice < 0 || choice >= static_cast<int>(targets.size())) {
        return -1;
    }
    
    return choice;
}

std::pair<int, int> GameSpellContext::requestCoordinates(const std::string& prompt) const {
    std::cout << prompt;
    
    int x, y;
    if (!(std::cin >> x >> y)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return {-1, -1};
    }
    
    return {x, y};
}

