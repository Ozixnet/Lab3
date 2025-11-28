#pragma once

#include "Magic/ISpellContext.h"
#include <utility>



class EntityManager;
class Board;
class GuiInputReader;
class ImprovedGuiRenderer;

class GuiSpellContext : public ISpellContext {
public:
    GuiSpellContext(EntityManager& em, Board& b, int gs, GuiInputReader* input, ImprovedGuiRenderer* renderer);

    // Базовые методы из ISpellContext
    std::pair<int, int> getPlayerPosition() const override;
    int getGridSize() const override;
    std::vector<TargetInfo> findTargetsInRadius(int centerX, int centerY, int radius) const override;
    bool dealDamage(int x, int y, int damage) override;
    bool isCellEmpty(int x, int y) const override;
    bool placeTrap(int x, int y, int damage) override;
    bool summonAlly(int x, int y, int health, int damage) override;
    std::pair<int, int> findFreeAdjacentCell(int centerX, int centerY) const override;
    SpellBuffContext& getBuffContext() override;
    const SpellBuffContext& getBuffContext() const override;

    // GUI-специфичные методы для выбора целей
    int requestTargetSelection(const std::vector<TargetInfo>& targets) const override;
    std::pair<int, int> requestCoordinates(const std::string& prompt) const override;

private:
    EntityManager& entityManager;
    Board& board;
    int gridSize;
    GuiInputReader* inputReader;
    ImprovedGuiRenderer* guiRenderer;
    
    // Визуальный выбор координат с помощью WASD + Enter
    std::pair<int, int> visualCoordinateSelection(const std::string& prompt) const;
};

