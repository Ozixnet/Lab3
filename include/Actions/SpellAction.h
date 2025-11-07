#ifndef SPELL_ACTION_H
#define SPELL_ACTION_H

#include "GameAction.h"

class SpellCard;
class EntityManager;
class Hand;

class SpellAction : public GameAction {
private:
    SpellCard* spell;
    EntityManager* entityManager;
    Hand* hand;
    int gridSize;
    int spellIndex;

public:
    SpellAction(SpellCard* s, EntityManager* em, Hand* h, int gs, int idx);
    int execute(Board& board, Player& player) override;

    // Геттер для доступа к индексу (нужен для удаления заклинания)
    int getSpellIndex() const { return spellIndex; }
};

#endif // SPELL_ACTION_H
