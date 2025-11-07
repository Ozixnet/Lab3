#include "Magic/Spells/TrapSpell.h"
#include "Magic/TrapRegistry.h"
#include "Magic/SpellParams.h"
#include "Entity/EntityManager.h"
#include "Board/Board.h"
#include <iostream>

// Статические члены класса (для совместимости, но не используются)
std::vector<std::unique_ptr<Trap>> TrapSpell::trapsOnField;
int TrapSpell::trapCount = 0;

TrapSpell::TrapSpell(int dmg, int rad, int maxTrp)
    : damage(dmg), radius(rad), maxTraps(maxTrp) {}

bool TrapSpell::use(EntityManager& entityManager, int gridSize) {
    try {
        // Создаем параметры и применяем баффы
        TrapParams params{damage};
        entityManager.getBuffContext().applyAndConsumeFor(params);
        
        int finalDamage = params.damage;
        
        // Получаем координаты игрока
        auto [playerX, playerY] = entityManager.getPlayerCoord();
        
        std::cout << "\n=== Заклинание Trap Spell ===" << std::endl;
        std::cout << "Выберите координаты для размещения ловушки (в пределах радиуса " 
                  << radius << "):" << std::endl;
        std::cout << "Введите координаты X Y (или -1 -1 для отмены): ";
        
        int targetX, targetY;
        if (!(std::cin >> targetX >> targetY)) {
            std::cout << "Ошибка ввода координат!" << std::endl;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            return false;
        }
        
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
        
        // Проверяем, что клетка пустая
        Board& board = entityManager.getBoard();
        Grid& grid = board.getGrid();
        CellType cell = grid.getCell(targetX, targetY);
        
        // Нельзя ставить на занятые клетки: стену, врага, здание, башню
        if (cell != CellType::Empty && cell != CellType::Player && cell != CellType::SlowTrap) {
            std::cout << "Нельзя поставить ловушку на занятую клетку!" << std::endl;
            return false;
        }
        
        // Проверка ловушки поля (slow trap): нельзя перекрывать
        if (cell == CellType::SlowTrap) {
            std::cout << "Нельзя поставить ловушку на ловушку поля!" << std::endl;
            return false;
        }
        
        // Проверить, что тут ещё нет игровой ловушки
        if (TrapRegistry::isTrapAt(targetX, targetY)) {
            std::cout << "Здесь уже есть ловушка!" << std::endl;
            return false;
        }
        
        // Размещаем ловушку через TrapRegistry с усиленным уроном
        if (!TrapRegistry::addTrap(targetX, targetY, finalDamage)) {
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

