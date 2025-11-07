#pragma once
#include "../SpellCard.h"

class EntityManager;

class EnhanceSpell : public SpellCard {
private:
    int potency;

public:
    explicit EnhanceSpell(int potency = 1);
    bool use(EntityManager& entityManager, int gridSize) override;
    const char* getName() const override { return "Enhance Spell"; }
};

