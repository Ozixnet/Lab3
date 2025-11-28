#include "Magic/GuiSpellContext.h"
#include "Entity/EntityManager.h"
#include "Board/Board.h"
#include "Board/Grid.h"
#include "Board/Cell.h"
#include "Input/GuiInputReader.h"
#include "Rendering/ImprovedGuiRenderer.h"
#include "Magic/TrapRegistry.h"
#include "Magic/SpellBuffContext.h"
#include <iostream>
#include <cmath>
#include <algorithm>

GuiSpellContext::GuiSpellContext(EntityManager& em, Board& b, int gs, GuiInputReader* input, ImprovedGuiRenderer* renderer)
    : entityManager(em), board(b), gridSize(gs), inputReader(input), guiRenderer(renderer) {}

std::pair<int, int> GuiSpellContext::getPlayerPosition() const {
    return entityManager.getPlayerCoord();
}

int GuiSpellContext::getGridSize() const {
    return gridSize;
}

std::vector<TargetInfo> GuiSpellContext::findTargetsInRadius(int centerX, int centerY, int radius) const {
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
        if (!buildings[idx].isActive()) continue;
        
        int bx = buildings[idx].getX();
        int by = buildings[idx].getY();
        int distX = std::abs(bx - centerX);
        int distY = std::abs(by - centerY);
        
        if (distX <= radius && distY <= radius) {
            TargetInfo target;
            target.x = bx;
            target.y = by;
            target.type = TargetInfo::Type::Building;
            target.index = static_cast<int>(idx);
            target.health = buildings[idx].getHealth();
            target.displayName = "Building #" + std::to_string(idx);
            targets.push_back(target);
        }
    }
    
    return targets;
}

bool GuiSpellContext::dealDamage(int x, int y, int damage) {
    int result = entityManager.handleAttackAt(x, y, damage);
    return result > 0; // Возвращаем true, если был нанесён урон
}

bool GuiSpellContext::isCellEmpty(int x, int y) const {
    if (x < 0 || x >= gridSize || y < 0 || y >= gridSize) {
        return false;
    }
    
    const Grid& grid = board.getGrid();
    if (!grid.isValidPosition(x, y)) {
        return false;
    }
    
    CellType cellType = grid.getCell(x, y);
    if (cellType == CellType::Wall) {
        return false;
    }
    
    // Проверяем, нет ли на клетке врагов, зданий, башен или союзников
    if (entityManager.isEnemyAt(x, y)) {
        return false;
    }
    
    if (entityManager.findBuildingAtPosition(x, y) != -1) {
        return false;
    }
    
    if (entityManager.findTowerAtPosition(x, y) != -1) {
        return false;
    }
    
    if (entityManager.findAllyAtPosition(x, y) != -1) {
        return false;
    }
    
    // Проверяем позицию игрока
    auto [px, py] = entityManager.getPlayerCoord();
    if (x == px && y == py) {
        return false;
    }
    
    return true;
}

bool GuiSpellContext::placeTrap(int x, int y, int damage) {
    if (x < 0 || x >= gridSize || y < 0 || y >= gridSize) {
        return false;
    }
    
    if (!isCellEmpty(x, y)) {
        return false;
    }
    
    return TrapRegistry::addTrap(x, y, damage);
}

bool GuiSpellContext::summonAlly(int x, int y, int health, int damage) {
    if (x < 0 || x >= gridSize || y < 0 || y >= gridSize) {
        return false;
    }
    
    if (!isCellEmpty(x, y)) {
        return false;
    }
    
    entityManager.addAlly(x, y, health, damage);
    return true;
}

std::pair<int, int> GuiSpellContext::findFreeAdjacentCell(int centerX, int centerY) const {
    return entityManager.findFreeAdjacentCell(centerX, centerY);
}

SpellBuffContext& GuiSpellContext::getBuffContext() {
    return entityManager.getBuffContext();
}

const SpellBuffContext& GuiSpellContext::getBuffContext() const {
    return entityManager.getBuffContext();
}

int GuiSpellContext::requestTargetSelection(const std::vector<TargetInfo>& targets) const {
    if (targets.empty()) {
        return -1;
    }
    
    std::cout << "\n=== Доступные цели (выберите цифрой) ===" << std::endl;
    for (size_t i = 0; i < targets.size(); ++i) {
        const auto& target = targets[i];
        std::cout << "[" << i << "] " << target.displayName 
                  << " на (" << target.x << ", " << target.y << ")"
                  << " HP: " << target.health << std::endl;
    }
    
    // Ждём ввода цифры через GUI
    while (true) {
        std::string input = inputReader->readRawInput();
        if (input.empty()) continue;
        
        char key = input[0];
        
        // ESC для отмены
        if (key == 27) {
            std::cout << "Выбор цели отменён\n";
            return -1;
        }
        
        // Цифры для выбора
        if (key >= '0' && key <= '9') {
            int choice = key - '0';
            if (choice >= 0 && choice < static_cast<int>(targets.size())) {
                std::cout << "Выбрана цель #" << choice << "\n";
                return choice;
            }
        }
    }
}

std::pair<int, int> GuiSpellContext::requestCoordinates(const std::string& prompt) const {
    std::cout << prompt << " (используйте WASD для перемещения курсора, Enter для подтверждения, ESC для отмены)\n";
    return visualCoordinateSelection(prompt);
}

std::pair<int, int> GuiSpellContext::visualCoordinateSelection(const std::string& /*prompt*/) const {
    // Начинаем с позиции игрока
    auto [cursorX, cursorY] = getPlayerPosition();
    
    std::cout << "[GuiSpellContext] Визуальный выбор координат. Начальная позиция: (" << cursorX << ", " << cursorY << ")\n";
    std::cout << "Используйте WASD для перемещения, Enter для подтверждения, ESC для отмены\n";
    
    // TODO: Добавить визуальное отображение курсора на сетке через renderer
    // Пока используем простой ввод
    
    while (true) {
        std::string input = inputReader->readRawInput();
        if (input.empty()) continue;
        
        char key = input[0];
        
        // ESC для отмены
        if (key == 27) {
            std::cout << "Выбор координат отменён\n";
            return {-1, -1};
        }
        
        // Enter для подтверждения
        if (key == '\n' || key == '\r') {
            std::cout << "Выбраны координаты: (" << cursorX << ", " << cursorY << ")\n";
            return {cursorX, cursorY};
        }
        
        // WASD для перемещения
        int newX = cursorX;
        int newY = cursorY;
        
        switch (key) {
            case 'w': newY--; break;
            case 's': newY++; break;
            case 'a': newX--; break;
            case 'd': newX++; break;
            default:
                continue;
        }
        
        // Проверяем границы
        if (newX >= 0 && newX < gridSize && newY >= 0 && newY < gridSize) {
            cursorX = newX;
            cursorY = newY;
            std::cout << "Курсор перемещён на (" << cursorX << ", " << cursorY << ")\n";
            
            // TODO: Обновить визуальное отображение курсора
            if (guiRenderer) {
                guiRenderer->forceRedraw();
            }
        }
    }
}

