#ifndef ENEMYBUILDING_H
#define ENEMYBUILDING_H

#include <utility>

class EnemyBuilding {
public:
    // Конструктор: координаты здания и количество ходов между спавнами
    EnemyBuilding(int x, int y, int spawnInterval, int id);

    // Уменьшает счетчик и проверяет, нужно ли создать врага
    bool shouldSpawn();

    // Сбрасывает таймер после спавна
    void resetTimer() noexcept;

    // Геттеры
    int getX() const { return x; }
    int getY() const { return y; }
    int getID() const { return id; }
    int getCurrentTimer() const { return currentTimer; }
    int getSpawnInterval() const { return spawnInterval; }
    bool isActive() const { return active; }
    int getHealth() const { return health; }

    // Установка активности (например, при разрушении)
    void setActive(bool state) { active = state; }
    void takeDamage(int amount) { health -= amount; }


private:
    int x, y;                    // Координаты здания
    int health;
    int id;
    int spawnInterval;           // Интервал в ходах между спавнами
    int currentTimer;            // Текущий счетчик ходов
    bool active;                 // Активно ли здание
};

#endif // ENEMYBUILDING_H
