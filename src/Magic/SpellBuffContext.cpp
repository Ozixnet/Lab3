#include "Magic/SpellBuffContext.h"
#include "Magic/SpellParams.h"

void SpellBuffContext::addStack(int potency) {
    stackCount += potency;
    // Распределяем вклад по компонентам
    directDamageRadiusBonus += potency;
    aoeExtraTiles += potency;
    trapDamageBonus += potency;
    summonExtraAllies += potency;
}

