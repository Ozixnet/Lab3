#pragma once
#include "Magic/SpellParams.h"

class SpellBuffContext {
private:
    int directDamageRadiusBonus = 0;
    int aoeExtraTiles = 0;
    int trapDamageBonus = 0;
    int summonExtraAllies = 0;
    int stackCount = 0;

public:
    // Добавить стек усиления
    void addStack(int potency = 1);
    
    // Применить и очистить баффы для конкретного заклинания
    template<typename Params>
    void applyAndConsumeFor(Params& params);
    
    // Сброс всех баффов (inline для использования в шаблонах)
    void clear() {
        directDamageRadiusBonus = 0;
        aoeExtraTiles = 0;
        trapDamageBonus = 0;
        summonExtraAllies = 0;
        stackCount = 0;
    }
    
    // Проверка наличия баффов
    bool hasBuffs() const { return stackCount > 0; }
};

// Специализации шаблона (должны быть в заголовочном файле для видимости)
template<>
inline void SpellBuffContext::applyAndConsumeFor<DirectDamageParams>(DirectDamageParams& p) {
    p.radius += directDamageRadiusBonus;
    clear();
}

template<>
inline void SpellBuffContext::applyAndConsumeFor<AoEParams>(AoEParams& p) {
    p.areaSize += aoeExtraTiles;
    clear();
}

template<>
inline void SpellBuffContext::applyAndConsumeFor<TrapParams>(TrapParams& p) {
    p.damage += trapDamageBonus;
    clear();
}

template<>
inline void SpellBuffContext::applyAndConsumeFor<SummonParams>(SummonParams& p) {
    p.count += summonExtraAllies;
    clear();
}

