#ifndef TRAP_SPELL_H
#define TRAP_SPELL_H

#include "../SpellCard.h"
#include <vector>
#include <memory>

class Trap;

/**
 * @brief Заклинание размещения ловушки
 * 
 * Позволяет разместить ловушку на игровом поле,
 * которая наносит урон врагам при срабатывании.
 */
class TrapSpell : public SpellCard {
private:
    int damage;
    int radius;
    int maxTraps;

public:
    TrapSpell(int dmg = 5, int rad = 5, int maxTrp = 3);
    
    bool use(ISpellContext& context) override;
    const char* getName() const override { return "Trap Spell"; }
    
    // Статические методы для совместимости (используются TrapRegistry)
    static void addTrap(int x, int y, int damage);
    static const std::vector<std::unique_ptr<Trap>>& getTraps();
    static void clearTraps();
    static int checkTrapAt(int x, int y);

private:
    static std::vector<std::unique_ptr<Trap>> trapsOnField;
    static int trapCount;
};

#endif // TRAP_SPELL_H
