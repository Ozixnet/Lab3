#ifndef SPELL_FACTORY_H
#define SPELL_FACTORY_H

#include "SpellCard.h"
#include <memory>
#include <string>

class SpellFactory {
public:
    // Создать заклинание по типу
    static std::unique_ptr<SpellCard> createSpell(const std::string& spellType);

    // Создать случайное заклинание
    static std::unique_ptr<SpellCard> createRandomSpell();

private:
    SpellFactory() = delete;  // Утилитарный класс, нельзя инстанцировать
};

#endif // SPELL_FACTORY_H
