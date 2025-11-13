#include "Magic/SpellFactory.h"
#include "Magic/Spells/DirectDamage.h"
#include "Magic/Spells/AreaDamage.h"
#include "Magic/Spells/TrapSpell.h"
#include "Magic/Spells/SummonSpell.h"
#include "Magic/Spells/EnhanceSpell.h"

#include <random>
#include <vector>

std::unique_ptr<SpellCard> SpellFactory::createSpell(const std::string& spellType) {
    if (spellType == "DirectDamage" || spellType == "direct" || spellType == "directdamage") {
        return std::make_unique<DirectDamage>();
    } else if (spellType == "AreaDamage" || spellType == "area" || spellType == "areadamage" || spellType == "aoe") {
        return std::make_unique<AreaDamage>();
    } else if (spellType == "TrapSpell" || spellType == "trap" || spellType == "trapspell") {
        return std::make_unique<TrapSpell>();
    } else if (spellType == "SummonSpell" || spellType == "summon" || spellType == "summonspell") {
        return std::make_unique<SummonSpell>();
    } else if (spellType == "EnhanceSpell" || spellType == "enhance" || spellType == "enhancespell") {
        return std::make_unique<EnhanceSpell>();
    }
    
    // Неизвестный тип
    return nullptr;
}

std::unique_ptr<SpellCard> SpellFactory::createRandomSpell() {
    // Список всех доступных типов заклинаний
    std::vector<std::string> spellTypes = {
        "DirectDamage",
        "AreaDamage",
        "TrapSpell",
        "SummonSpell",
        "EnhanceSpell"
    };
    
    // Случайный выбор типа
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, spellTypes.size() - 1);
    
    std::string chosenType = spellTypes[dist(gen)];
    return createSpell(chosenType);
}
