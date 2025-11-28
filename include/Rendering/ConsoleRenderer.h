#pragma once

#include "Rendering/IGameRenderer.h"
#include <string>



class Board;
class Player;
class Hand;
class Level;
class KeyBindings;

/**
 * @brief Реализация IGameRenderer для консольной отрисовки
 * 
 * @requirement Требование 2: Реализация класса отрисовки игры
 * 
 * Используется в GameView<ConsoleRenderer> (требование 4).
 * Использует KeyBindings для отображения подсказок управления (требование 5).
 * 
 * @see IGameRenderer
 * @see GameView
 * @see KeyBindings
 */
class ConsoleRenderer : public IGameRenderer {
public:
    void renderGameScreen(
        const Board* board,
        const Player* player,
        const Hand* hand,
        int moveCount
    ) override;

    void renderOptionsMenu() override;
    void renderUpgradeMenu(const Player* player, const Hand* hand) override;
    void renderMainMenu(bool hasSave) override;

    void renderLevelStart(const Level* level) override;
    void renderLevelComplete(const Level* level, int moves, int hp) override;
    void renderGameComplete(int finalHp) override;

    void renderGameOver(int moveCount) override;
    void renderVictory(int moveCount, int playerHealth) override;

    void clearScreen() override;
    void pause(int milliseconds) override;

    void setKeyBindings(const KeyBindings* bindings) { keyBindings = bindings; }

private:
    void renderGameStats(const Board* board, const Player* player, int moveCount);
    void renderSpellMenu(const Hand* hand);
    void renderSeparator(int width);
    void renderBorder(const std::string& content, int width);
    std::string getControlsHelp() const;
    std::string getConfigInfo() const;

    const KeyBindings* keyBindings{nullptr};

    static constexpr int WINDOW_WIDTH = 37;
    static constexpr int WINDOW_HEIGHT = 25;
};



