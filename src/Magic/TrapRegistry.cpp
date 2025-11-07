#include "Magic/TrapRegistry.h"
#include <algorithm>

std::vector<TrapInstance> TrapRegistry::traps;

bool TrapRegistry::addTrap(int x, int y, int damage) {
    // Проверка на дубликат
    for (const auto& trap : traps) {
        if (trap.x == x && trap.y == y && trap.active) {
            return false;
        }
    }
    
    traps.push_back({x, y, damage, true});
    return true;
}

int TrapRegistry::triggerAt(int x, int y) {
    for (auto it = traps.begin(); it != traps.end(); ++it) {
        if (it->x == x && it->y == y && it->active) {
            int damage = it->damage;
            traps.erase(it);  // Удаляем ловушку после срабатывания
            return damage;
        }
    }
    return 0;
}

bool TrapRegistry::isTrapAt(int x, int y) {
    for (const auto& trap : traps) {
        if (trap.x == x && trap.y == y && trap.active) {
            return true;
        }
    }
    return false;
}

const std::vector<TrapInstance>& TrapRegistry::getAll() {
    return traps;
}

void TrapRegistry::clear() {
    traps.clear();
}

