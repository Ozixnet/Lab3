#ifndef AREA_DAMAGE_H
#define AREA_DAMAGE_H

#include "../SpellCard.h"

class AreaDamage : public SpellCard {
private:
    int damage;
    int radius;

public:
    AreaDamage(int dmg = 2, int rad = 3);
    
    bool use(EntityManager& entityManager, int gridSize) override;
    const char* getName() const override;
    
    // Геттеры для параметров
    int getDamage() const { return damage; }
    int getRadius() const { return radius; }
};

#endif // AREA_DAMAGE_H
