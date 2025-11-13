#include "Magic/Spells/TrapSpell.h"
#include "Magic/ISpellContext.h"
#include "Magic/SpellParams.h"
#include "Magic/SpellBuffContext.h"
#include "Board/Trap.h"
#include <iostream>
#include <cmath>

// Статические члены класса (для совместимости)
std::vector<std::unique_ptr<Trap>> TrapSpell::trapsOnField;
int TrapSpell::trapCount = 0;

TrapSpell::TrapSpell(int dmg, int rad, int maxTrp)
    : damage(dmg), radius(rad), maxTraps(maxTrp) {}

bool TrapSpell::use(ISpellContext& context) {
    try {
        // Создаем параметры и применяем баффы
        TrapParams params{damage};
        context.getBuffContext().applyAndConsumeFor(params);
        
        int finalDamage = params.damage;
        
        // Получаем координаты игрока
        auto [playerX, playerY] = context.getPlayerPosition();
        int gridSize = context.getGridSize();
        
        std::cout << "\n=== Заклинание Trap Spell ===" << std::endl;
        std::cout << "Выберите координаты для размещения ловушки (в пределах радиуса " 
                  << radius << "):" << std::endl;
        
        auto [targetX, targetY] = context.requestCoordinates(
            "Введите координаты X Y (или -1 -1 для отмены): "
        );
        
        if (targetX == -1 && targetY == -1) {
            std::cout << "Размещение ловушки отменено." << std::endl;
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
        if (targetX < 0 || targetX >= gridSize || targetY < 0 || targetY >= gridSize) {
            std::cout << "Координаты выходят за границы сетки!" << std::endl;
            return false;
        }
        
        // Проверяем через контекст, что клетка подходит
        if (!context.isCellEmpty(targetX, targetY)) {
            std::cout << "Нельзя поставить ловушку на занятую клетку!" << std::endl;
            return false;
        }
        
        // Проверяем, что тут нет ловушки
        if (context.hasTrapAt(targetX, targetY)) {
            std::cout << "Здесь уже есть ловушка!" << std::endl;
            return false;
        }
        
        // Размещаем ловушку через контекст
        if (!context.placeTrap(targetX, targetY, finalDamage)) {
            std::cout << "Не удалось разместить ловушку!" << std::endl;
            return false;
        }
        
        std::cout << "✓ Ловушка размещена на (" << targetX << ", " << targetY 
                  << ") с уроном " << finalDamage << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in TrapSpell::use: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown error in TrapSpell::use" << std::endl;
        return false;
    }
}

// Статические методы для совместимости
void TrapSpell::addTrap(int x, int y, int damage) {
    trapsOnField.push_back(std::make_unique<Trap>(x, y, damage));
    trapCount++;
}

const std::vector<std::unique_ptr<Trap>>& TrapSpell::getTraps() {
    return trapsOnField;
}

void TrapSpell::clearTraps() {
    trapsOnField.clear();
    trapCount = 0;
}

int TrapSpell::checkTrapAt(int x, int y) {
    for (auto& trap : trapsOnField) {
        if (trap && trap->isAt(x, y) && trap->isActivated()) {
            int dmg = trap->trigger();
            return dmg;
        }
    }
    return 0;
}
