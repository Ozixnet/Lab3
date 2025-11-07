#pragma once
#include "../SpellCard.h"

class EntityManager;

class SummonSpell : public SpellCard {
private:
    int baseCount;
    int allyHp;
    int allyDmg;

public:
    explicit SummonSpell(int baseCount = 1, int allyHp = 3, int allyDmg = 1);
    bool use(EntityManager& entityManager, int gridSize) override;
    const char* getName() const override { return "Summon Spell"; }
};

