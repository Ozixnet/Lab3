#include "Magic/Spells/AreaDamage.h"
#include "Magic/SpellCard.h"
#include "Magic/SpellParams.h"
#include "Entity/EntityManager.h"
#include <iostream>
#include <cmath>
#include <vector>

// Конструктор
AreaDamage::AreaDamage(int dmg, int rad)
    : damage(dmg), radius(rad) {}

// Реализация заклинания урона по площади
bool AreaDamage::use(EntityManager& entityManager, int gridSize) {
    // Создаем параметры и применяем баффы
    AoEParams params{damage, 2};  // Базовый размер области 2x2
    entityManager.getBuffContext().applyAndConsumeFor(params);
    
    int finalDamage = params.damage;
    int finalAreaSize = params.areaSize;
    
    // Получаем координаты игрока
    auto [playerX, playerY] = entityManager.getPlayerCoord();

    std::cout << "\n=== Заклинание Area Damage (" << finalAreaSize << "x" << finalAreaSize << ") ===" << std::endl;
    std::cout << "Выберите левый верхний угол области " << finalAreaSize << "x" << finalAreaSize 
              << " (в пределах радиуса " << radius << "):" << std::endl;
    std::cout << "Введите координаты X Y (или -1 -1 для отмены): ";

    int targetX, targetY;
    std::cin >> targetX >> targetY;

    if (targetX == -1 && targetY == -1) {
        std::cout << "Заклинание отменено." << std::endl;
        return false;
    }

    // Проверяем радиус
    int distX = std::abs(targetX - playerX);
    int distY = std::abs(targetY - playerY);
    if (distX > radius || distY > radius) {
        std::cout << "Цель слишком далеко! Максимальный радиус: " << radius << std::endl;
        return false;
    }

    // Проверяем границы сетки (область finalAreaSize x finalAreaSize)
    if (targetX < 0 || targetX + finalAreaSize - 1 >= gridSize ||
        targetY < 0 || targetY + finalAreaSize - 1 >= gridSize) {
        std::cout << "Координаты выходят за границы сетки!" << std::endl;
        return false;
    }

    // Генерируем координаты области finalAreaSize x finalAreaSize
    std::vector<std::pair<int, int>> areaCoords;
    for (int dy = 0; dy < finalAreaSize; ++dy) {
        for (int dx = 0; dx < finalAreaSize; ++dx) {
            areaCoords.push_back({targetX + dx, targetY + dy});
        }
    }

    std::cout << "Применяем Area Damage в области:" << std::endl;
    for (const auto& [x, y] : areaCoords) {
        std::cout << "  (" << x << ", " << y << ")" << std::endl;
    }

    // Наносим урон всем сущностям в области
    int totalDamaged = 0;

    for (const auto& [aoeX, aoeY] : areaCoords) {
        int result = entityManager.handleAttackAt(aoeX, aoeY, finalDamage);
        if (result > 0) {
            std::cout << "Урон по цели на (" << aoeX << ", " << aoeY << "): "
                     << finalDamage << " HP" << std::endl;
            totalDamaged++;
        }
    }

    std::cout << "Всего поражено целей: " << totalDamaged << std::endl;
    return true;
}

// Возврат имени
const char* AreaDamage::getName() const {
    return "Area Damage";
}
