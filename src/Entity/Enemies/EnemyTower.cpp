#include "Entity/Enemies/EnemyTower.h"
#include "Magic/Spells/DirectDamage.h"
#include "Magic/GameSpellContext.h"
#include "Entity/EntityManager.h"
#include "Board/Board.h"
#include <iostream>
#include <cmath>

int EnemyTower::nextTowerId = 1;

EnemyTower::EnemyTower(int x, int y, int health, int radius, int damage, int cd)
    : id(nextTowerId++), x(x), y(y), health(health), maxHealth(health),
      attackRadius(radius), attackDamage(damage), cooldownMax(cd), cooldownCur(0) {
    // Создаем DirectDamage с авто-таргетом для башни
    spell = std::make_unique<DirectDamage>(attackDamage, attackRadius, true, x, y);
}

// Конструктор копирования
EnemyTower::EnemyTower(const EnemyTower& other)
    : id(other.id), x(other.x), y(other.y), health(other.health), maxHealth(other.maxHealth),
      attackRadius(other.attackRadius), attackDamage(other.attackDamage),
      cooldownMax(other.cooldownMax), cooldownCur(other.cooldownCur) {
    // Создаем новый DirectDamage с теми же параметрами
    if (other.spell) {
        spell = std::make_unique<DirectDamage>(attackDamage, attackRadius, true, x, y);
    }
}

// Конструктор перемещения
EnemyTower::EnemyTower(EnemyTower&& other) noexcept
    : id(other.id), x(other.x), y(other.y), health(other.health), maxHealth(other.maxHealth),
      attackRadius(other.attackRadius), attackDamage(other.attackDamage),
      cooldownMax(other.cooldownMax), cooldownCur(other.cooldownCur),
      spell(std::move(other.spell)) {
}

// Оператор присваивания копированием
EnemyTower& EnemyTower::operator=(const EnemyTower& other) {
    if (this != &other) {
        id = other.id;
        x = other.x;
        y = other.y;
        health = other.health;
        maxHealth = other.maxHealth;
        attackRadius = other.attackRadius;
        attackDamage = other.attackDamage;
        cooldownMax = other.cooldownMax;
        cooldownCur = other.cooldownCur;
        // Создаем новый DirectDamage с теми же параметрами
        if (other.spell) {
            spell = std::make_unique<DirectDamage>(attackDamage, attackRadius, true, x, y);
        }
    }
    return *this;
}

// Оператор присваивания перемещением
EnemyTower& EnemyTower::operator=(EnemyTower&& other) noexcept {
    if (this != &other) {
        id = other.id;
        x = other.x;
        y = other.y;
        health = other.health;
        maxHealth = other.maxHealth;
        attackRadius = other.attackRadius;
        attackDamage = other.attackDamage;
        cooldownMax = other.cooldownMax;
        cooldownCur = other.cooldownCur;
        spell = std::move(other.spell);
    }
    return *this;
}

void EnemyTower::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
    std::cout << "⚔ Башня [" << id << "] получила " << damage << " урона! HP: " 
              << health << "/" << maxHealth << "\n";
}

bool EnemyTower::isPlayerInRange(int playerX, int playerY) const {
    int distX = std::abs(playerX - x);
    int distY = std::abs(playerY - y);
    
    // Используем Chebyshev distance (максимум из двух расстояний)
    int maxDist = std::max(distX, distY);
    return maxDist <= attackRadius;
}

bool EnemyTower::tryAttack(EntityManager& em, Board& board, int gridSize, int playerX, int playerY) {
    if (cooldownCur > 0) {
        return false;  // На кулдауне, не атакует
    }
    
    // Проверить игрока в радиусе (Chebyshev)
    int dx = std::abs(playerX - x);
    int dy = std::abs(playerY - y);
    if (std::max(dx, dy) > attackRadius) {
        return false;  // Игрок вне радиуса
    }
    
    // Обновить координаты цели в spell (для авто-таргета)
    // Создаем новый DirectDamage с координатами игрока
    spell = std::make_unique<DirectDamage>(attackDamage, attackRadius, true, playerX, playerY);
    
    // Создаем контекст для заклинания башни
    GameSpellContext context(em, board, gridSize);
    
    // Выполнить spell через контекст
    bool used = spell->use(context);
    
    if (used) {
        cooldownCur = cooldownMax;  // Установить кулдаун после успешной атаки
        return true;
    }
    
    return false;
}
