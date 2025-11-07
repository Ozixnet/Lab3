#include "Entity/Enemies/Enemy.h"
#include <iostream>

Enemy::Enemy(int h, int d, int id)
    : id(id), health(h), damage(d) {}

void Enemy::displayInfo() const {
    std::cout << "Enemy #" << id
              << " | Health: " << health
              << ", Damage: " << damage
              << std::endl;
}
