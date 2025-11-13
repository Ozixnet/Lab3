// include/Magic/Spells/DirectDamage.h
#ifndef DIRECT_DAMAGE_H
#define DIRECT_DAMAGE_H

#include "../SpellCard.h"

/**
 * @brief Заклинание прямого урона по одной цели
 * 
 * Позволяет нанести урон одной цели в заданном радиусе.
 * Поддерживает режим авто-таргета для башен.
 */
class DirectDamage : public SpellCard {
private:
    int damage;
    int radius;
    bool autoAim;           // Режим авто-таргета для башен
    int targetX, targetY;   // Координаты цели в режиме авто-таргета

public:
    explicit DirectDamage(int dmg = 5, int rad = 1);
    DirectDamage(int dmg, int rad, bool autoAim, int tx, int ty);  // Конструктор для авто-таргета

    bool use(ISpellContext& context) override;
    const char* getName() const override;

    int getDamage() const { return damage; }
    int getRadius() const { return radius; }
};

#endif // DIRECT_DAMAGE_H
