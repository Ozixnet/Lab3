#include "Board/Trap.h"
#include <iostream>

int Trap::nextId = 1;

// Конструктор для SlowTrap (для игрока)
Trap::Trap(int x, int y, int slowDuration)
    : id(nextId++), x(x), y(y), type(TrapType::SlowTrap),
      damage(0), slowDuration(slowDuration), isActive(true) {}

// Конструктор для PlayerTrap (наносит урон врагам)
Trap::Trap(int x, int y, int damage, TrapType type)
    : id(nextId++), x(x), y(y), type(type),
      damage(damage), slowDuration(0), isActive(true) {}

int Trap::trigger() {
    if (!isActive) return 0;

    // === ВАЖНО: SlowTrap НЕ наносит урон врагам ===
    if (type == TrapType::SlowTrap) {
        // SlowTrap срабатывает только для игрока, не для врагов
        return 0;
    }

    // === PlayerTrap НАНОСИТ УРОН ===
    if (type == TrapType::PlayerTrap) {
        isActive = false;
        std::cout << "💥 Враг попал на ловушку игрока! Урон: " << damage << "\n";
        return damage;
    }

    return 0;
}
