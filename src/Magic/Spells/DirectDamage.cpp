#include "Magic/Spells/DirectDamage.h"
#include "Magic/SpellCard.h"
#include "Magic/SpellParams.h"
#include "Entity/EntityManager.h"

#include <algorithm>
#include <iostream>
#include <cmath>

// Конструктор
DirectDamage::DirectDamage(int dmg, int rad)
    : damage(dmg), radius(rad), autoAim(false), targetX(0), targetY(0) {}

// Конструктор для авто-таргета (для башен)
DirectDamage::DirectDamage(int dmg, int rad, bool autoAim, int tx, int ty)
    : damage(dmg), radius(rad), autoAim(autoAim), targetX(tx), targetY(ty) {}

// Реализация заклинания прямого урона
bool DirectDamage::use(EntityManager& entityManager, int gridSize) {
    // Если авто-таргет (для башен) - сразу атакуем цель (без баффов)
    if (autoAim) {
        int result = entityManager.handleAttackAt(targetX, targetY, damage);
        if (result >= 0) {
            std::cout << "Башня атакует цель на (" << targetX << ", " << targetY << ") уроном " << damage << "!\n";
            return true;
        }
        return false;
    }
    
    // Создаем параметры и применяем баффы
    DirectDamageParams params{damage, radius};
    entityManager.getBuffContext().applyAndConsumeFor(params);
    
    // Используем модифицированные параметры
    int finalDamage = params.damage;
    int finalRadius = params.radius;
    
    // Получаем координаты игрока из EntityManager
    auto [playerX, playerY] = entityManager.getPlayerCoord();

    // Получаем всех врагов и здания напрямую (не через enemyScanner, который сканирует только 3x3)
    const auto& enemies = entityManager.getEnemies();
    const auto& enemyCoords = entityManager.getEnemyCoords();
    const auto& buildings = entityManager.getBuildings();

    // Фильтруем цели по радиусу - проверяем ВСЕХ врагов и здания
    std::vector<int> validEnemies;
    std::vector<int> validBuildings;

    // Проверяем всех врагов в радиусе
    for (size_t idx = 0; idx < enemies.size(); ++idx) {
        // Проверяем, что враг жив
        if (!enemies[idx].IsAlive()) {
            continue;
        }
        
        if (idx < enemyCoords.size()) {
            auto [ex, ey] = enemyCoords[idx];
            int distX = std::abs(ex - playerX);
            int distY = std::abs(ey - playerY);
            // Проверяем квадратный радиус (как в задании - "достижимый радиус")
            if (distX <= finalRadius && distY <= finalRadius) {
                validEnemies.push_back(static_cast<int>(idx));
            }
        }
    }

    // Проверяем все здания в радиусе
    for (size_t idx = 0; idx < buildings.size(); ++idx) {
        const auto& building = buildings[idx];
        // Проверяем, что здание активно и имеет HP
        if (!building.isActive() || building.getHealth() <= 0) {
            continue;
        }
        
        int bx = building.getX();
        int by = building.getY();
        int distX = std::abs(bx - playerX);
        int distY = std::abs(by - playerY);
        // Проверяем, что здание в радиусе
        if (distX <= finalRadius && distY <= finalRadius) {
            validBuildings.push_back(static_cast<int>(idx));
        }
    }

    // Если нет целей в радиусе - заклинание не используется
    if (validEnemies.empty() && validBuildings.empty()) {
        std::cout << "В радиусе действия (" << finalRadius << ") нет целей!" << std::endl;
        return false;
    }

    // Выводим список доступных целей
    std::cout << "\n=== Доступные цели в радиусе " << finalRadius << " ===" << std::endl;

    std::cout << "Враги:" << std::endl;
    for (int idx : validEnemies) {
        auto [ex, ey] = entityManager.getEnemyCoords()[idx];
        std::cout << "  [E" << idx << "] Позиция: (" << ex << ", " << ey << ")" << std::endl;
    }

    std::cout << "Здания:" << std::endl;
    for (int idx : validBuildings) {
        const auto& building = buildings[idx];
        std::cout << "  [B" << idx << "] Позиция: (" << building.getX() << ", " << building.getY() 
                  << ") HP: " << building.getHealth() << std::endl;
    }

    // Цикл выбора цели
    while (true) {
        std::cout << "\nВведите цель (E<номер> для врага, B<номер> для здания, Q для отмены): ";
        char type;
        std::cin >> type;

        if (type == 'Q' || type == 'q') {
            std::cout << "Заклинание отменено." << std::endl;
            return false;
        }

        int index;
        std::cin >> index;

        if (type == 'E' || type == 'e') {
            auto it = std::find(validEnemies.begin(), validEnemies.end(), index);
            if (it != validEnemies.end()) {
                auto [ex, ey] = entityManager.getEnemyCoords()[index];
                std::cout << "Атакуем врага [E" << index << "] на " << finalDamage << " урона!" << std::endl;
                entityManager.handleAttackAt(ex, ey, finalDamage);
                return true;
            } else {
                std::cout << "Некорректный или недоступный индекс врага!" << std::endl;
            }
        }
        else if (type == 'B' || type == 'b') {
            auto it = std::find(validBuildings.begin(), validBuildings.end(), index);
            if (it != validBuildings.end()) {
                auto& building = buildings[index];
                std::cout << "Атакуем здание [B" << index << "] на " << finalDamage << " урона!" << std::endl;
                entityManager.handleAttackAt(building.getX(), building.getY(), finalDamage);
                return true;
            } else {
                std::cout << "Некорректный или недоступный индекс здания!" << std::endl;
            }
        }
        else {
            std::cout << "Неверный тип! Используйте E (враг) или B (здание)." << std::endl;
        }
    }
}

// Возврат имени
const char* DirectDamage::getName() const {
    return "Direct Damage";
}

