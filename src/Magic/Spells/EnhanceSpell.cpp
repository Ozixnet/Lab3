#include "Magic/Spells/EnhanceSpell.h"
#include "Magic/ISpellContext.h"
#include "Magic/SpellBuffContext.h"
#include <iostream>

EnhanceSpell::EnhanceSpell(int potency) : potency(potency) {}

bool EnhanceSpell::use(ISpellContext& context) {
    try {
        std::cout << "\n=== Заклинание Enhance Spell ===" << std::endl;
        
        context.getBuffContext().addStack(potency);
        
        std::cout << "✓ Усиление накоплено! Мощность: " << potency << std::endl;
        std::cout << "Следующее заклинание будет усилено." << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in EnhanceSpell::use: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown error in EnhanceSpell::use" << std::endl;
        return false;
    }
}
