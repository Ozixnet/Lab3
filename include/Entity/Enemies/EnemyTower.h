#ifndef ENEMY_TOWER_H
#define ENEMY_TOWER_H

#include <memory>


class EntityManager;
class SpellCard;

class EnemyTower {
private:
    int id;
    int x, y;                    // Координаты на поле
    int health;
    int maxHealth;
    int attackRadius;            // Радиус атаки
    int attackDamage;            // Урон ослабленный (половина обычного)
    int cooldownMax;             // Максимальный кулдаун
    int cooldownCur;             // Текущий счетчик кулдауна
    std::unique_ptr<SpellCard> spell;  // DirectDamage для атаки
    static int nextTowerId;

public:
    EnemyTower(int x, int y, int health = 2, int radius = 1, int damage = 1, int cd = 20);
    
    // Конструкторы копирования и перемещения
    EnemyTower(const EnemyTower& other);
    EnemyTower(EnemyTower&& other) noexcept;
    EnemyTower& operator=(const EnemyTower& other);
    EnemyTower& operator=(EnemyTower&& other) noexcept;

    // === ГЕТТЕРЫ ===
    int getId() const { return id; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAttackRadius() const { return attackRadius; }
    int getAttackDamage() const { return attackDamage; }
    int getCooldown() const { return cooldownCur; }

    // сеттеры
    void takeDamage(int damage);
    bool isAlive() const { return health > 0; }

    // === ЛОГИКА БАШНИ ===

    // Обновить кулдаун (вызывается каждый ход)
    void tickCooldown() { if (cooldownCur > 0) --cooldownCur; }

    // Проверить, может ли башня атаковать сейчас
    bool canAttackNow() const { return cooldownCur == 0; }

    // Проверить, находится ли игрок в радиусе атаки
    bool isPlayerInRange(int playerX, int playerY) const;

    // Попытаться атаковать игрока через spell
    bool tryAttack(EntityManager& em, class Board& board, int gridSize, int playerX, int playerY);
};

#endif // ENEMY_TOWER_H
