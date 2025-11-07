#ifndef TRAP_H
#define TRAP_H

enum class TrapType {
    SlowTrap,      // Замедляющая ловушка (ТОЛЬКО для игрока, не влияет на врагов)
    PlayerTrap     // Ловушка игрока (НАНОСИТ УРОН врагам)
};

class Trap {
private:
    int id;
    int x, y;
    TrapType type;
    int damage;              // Для PlayerTrap
    int slowDuration;        // Для SlowTrap
    bool isActive;
    static int nextId;

public:
    // Конструктор для SlowTrap
    Trap(int x, int y, int slowDuration = 2);

    // Конструктор для PlayerTrap
    Trap(int x, int y, int damage, TrapType type);

    // === ГЕТТЕРЫ ===
    int getId() const { return id; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getDamage() const { return damage; }
    int getSlowDuration() const { return slowDuration; }
    TrapType getType() const { return type; }
    bool isActivated() const { return isActive; }
    bool isAt(int px, int py) const { return x == px && y == py; }

    // === ВЗАИМОДЕЙСТВИЕ ===
    // Срабатывание ловушки (для врагов только PlayerTrap!)
    int trigger();
    void deactivate() { isActive = false; }
};

#endif // TRAP_H
