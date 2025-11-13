#ifndef AREA_DAMAGE_H
#define AREA_DAMAGE_H

#include "../SpellCard.h"

/**
 * @brief Заклинание урона по площади
 * 
 * Наносит урон всем целям в выбранной квадратной области.
 */
class AreaDamage : public SpellCard {
private:
    int damage;
    int radius;

public:
    AreaDamage(int dmg = 2, int rad = 3);
    
    bool use(ISpellContext& context) override;
    const char* getName() const override;
    
    int getDamage() const { return damage; }
    int getRadius() const { return radius; }
};

#endif // AREA_DAMAGE_H
