#ifndef GAME_SPELL_CONTEXT_H
#define GAME_SPELL_CONTEXT_H

#include "ISpellContext.h"

// Forward declarations
class EntityManager;
class Board;

/**
 * @brief Реальная реализация ISpellContext для игры
 * 
 * Эта реализация делегирует все вызовы к реальным EntityManager и Board.
 * Вся сложная логика взаимодействия с игровым миром находится здесь,
 * а не в заклинаниях.
 */
class GameSpellContext : public ISpellContext {
private:
    EntityManager& entityManager;
    Board& board;
    int gridSize;
    
public:
    GameSpellContext(EntityManager& em, Board& b, int gs);
    
    // Реализация интерфейса
    std::pair<int, int> getPlayerPosition() const override;
    int getGridSize() const override;
    std::vector<TargetInfo> findTargetsInRadius(int centerX, int centerY, int radius) const override;
    bool isCellEmpty(int x, int y) const override;
    bool hasTrapAt(int x, int y) const override;
    
    bool dealDamage(int x, int y, int damage) override;
    bool dealDamageToTarget(const TargetInfo& target, int damage) override;
    bool placeTrap(int x, int y, int damage) override;
    bool summonAlly(int x, int y, int health, int damage) override;
    std::pair<int, int> findFreeAdjacentCell(int centerX, int centerY) const override;
    
    SpellBuffContext& getBuffContext() override;
    const SpellBuffContext& getBuffContext() const override;
    
    int requestTargetSelection(const std::vector<TargetInfo>& targets) const override;
    std::pair<int, int> requestCoordinates(const std::string& prompt) const override;
};

#endif // GAME_SPELL_CONTEXT_H

