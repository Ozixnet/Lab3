#ifndef SUMMON_SPELL_H
#define SUMMON_SPELL_H

#include "../SpellCard.h"

/**
 * @brief Заклинание призыва союзников
 * 
 * Призывает союзников рядом с игроком для помощи в бою.
 */
class SummonSpell : public SpellCard {
private:
    int baseCount;
    int allyHp;
    int allyDmg;

public:
    explicit SummonSpell(int baseCount = 1, int allyHp = 3, int allyDmg = 1);
    bool use(ISpellContext& context) override;
    const char* getName() const override { return "Summon Spell"; }
};

#endif // SUMMON_SPELL_H
