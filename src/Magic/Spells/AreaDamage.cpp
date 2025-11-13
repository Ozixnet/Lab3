#include "Magic/Spells/AreaDamage.h"
#include "Magic/ISpellContext.h"
#include "Magic/SpellParams.h"
#include "Magic/SpellBuffContext.h"
#include <iostream>
#include <cmath>
#include <vector>

AreaDamage::AreaDamage(int dmg, int rad)
    : damage(dmg), radius(rad) {}

bool AreaDamage::use(ISpellContext& context) {
    // Создаем параметры и применяем баффы
    AoEParams params{damage, 2};  // Базовый размер области 2x2
    context.getBuffContext().applyAndConsumeFor(params);
    
    int finalDamage = params.damage;
    int finalAreaSize = params.areaSize;
    
    // Получаем координаты игрока
    auto [playerX, playerY] = context.getPlayerPosition();
    int gridSize = context.getGridSize();
    
    std::cout << "\n=== Заклинание Area Damage (" << finalAreaSize << "x" << finalAreaSize << ") ===" << std::endl;
    std::cout << "Выберите левый верхний угол области " << finalAreaSize << "x" << finalAreaSize 
              << " (в пределах радиуса " << radius << "):" << std::endl;
    
    auto [targetX, targetY] = context.requestCoordinates("Введите координаты X Y (или -1 -1 для отмены): ");
    
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
    
    // Проверяем границы сетки
    if (targetX < 0 || targetX + finalAreaSize - 1 >= gridSize ||
        targetY < 0 || targetY + finalAreaSize - 1 >= gridSize) {
        std::cout << "Координаты выходят за границы сетки!" << std::endl;
        return false;
    }
    
    // Генерируем координаты области
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
        if (context.dealDamage(aoeX, aoeY, finalDamage)) {
            std::cout << "Урон по цели на (" << aoeX << ", " << aoeY << "): "
                     << finalDamage << " HP" << std::endl;
            totalDamaged++;
        }
    }
    
    std::cout << "Всего поражено целей: " << totalDamaged << std::endl;
    return true;
}

const char* AreaDamage::getName() const {
    return "Area Damage";
}
