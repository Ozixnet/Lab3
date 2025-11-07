#ifndef SPELL_CARD_H
#define SPELL_CARD_H

class EntityManager;

class SpellCard {
public:
    virtual ~SpellCard() = default;

    virtual bool use(EntityManager& entityManager, int gridSize) = 0;
    virtual const char* getName() const = 0;
};

#endif // SPELL_CARD_H
