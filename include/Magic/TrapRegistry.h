#pragma once

#include <vector>

struct TrapInstance {
    int x;
    int y;
    int damage;
    bool active;
};

class TrapRegistry {
public:
    static bool addTrap(int x, int y, int damage);          // добавить если пустая клетка
    static int triggerAt(int x, int y);                      // если есть активная ловушка: вернуть урон и деактивировать (и удалить)
    static bool isTrapAt(int x, int y);
    static const std::vector<TrapInstance>& getAll();
    static void clear();

private:
    static std::vector<TrapInstance> traps;
};

