#include "Entity/Player/Player.h"
#include <iostream>

Player::Player(int h, int d)
    : health(h), maxHealth(h), damage(d), baseDamage(d) {}

void Player::displayInfo() const {
    std::cout << "=== Player Stats ===" << std::endl;
    std::cout << "Health: " << health << " / " << maxHealth << std::endl;
    std::cout << "Damage: " << damage << std::endl;
    std::cout << "Distance: " << distance << std::endl;

    if (slowed) {
        std::cout << "STATUS: ЗАМЕДЛЕН! (Пропуск следующего хода)" << std::endl;
    }

    std::cout << "====================" << std::endl;
}
