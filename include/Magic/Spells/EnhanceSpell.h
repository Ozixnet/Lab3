#ifndef ENHANCE_SPELL_H
#define ENHANCE_SPELL_H

#include "../SpellCard.h"

/**
 * @brief Заклинание усиления
 * 
 * Добавляет стек усиления, который улучшит следующее заклинание.
 */
class EnhanceSpell : public SpellCard {
private:
    int potency;

public:
    explicit EnhanceSpell(int potency = 1);
    bool use(ISpellContext& context) override;
    const char* getName() const override { return "Enhance Spell"; }
};

#endif // ENHANCE_SPELL_H
