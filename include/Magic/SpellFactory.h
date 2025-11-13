#ifndef SPELL_FACTORY_H
#define SPELL_FACTORY_H

#include "SpellCard.h"
#include <memory>
#include <string>

class SpellFactory {
public:
    // Создать заклинание
    static std::unique_ptr<SpellCard> createSpell(const std::string& spellType);

    // случайное заклинание
    static std::unique_ptr<SpellCard> createRandomSpell();

private:
    SpellFactory() = delete;
};

#endif // SPELL_FACTORY_H
