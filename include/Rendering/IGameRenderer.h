#pragma once



class Board;
class Player;
class Hand;
class Level;

/**
 * @brief Интерфейс для отрисовки игры
 * 
 * @requirement Требование 2: Создать класс отрисовки игры. Данный класс определяет то, как должно отображаться игра.
 * 
 * Реализации:
 * - ConsoleRenderer - консольная отрисовка
 * - ImprovedGuiRenderer - GUI отрисовка
 * 
 * @see ConsoleRenderer
 * @see ImprovedGuiRenderer
 * @see GameView
 */
class IGameRenderer {
public:
    virtual ~IGameRenderer() = default;

    virtual void renderGameScreen(
        const Board* board,
        const Player* player,
        const Hand* hand,
        int moveCount
    ) = 0;

    virtual void renderOptionsMenu() = 0;
    virtual void renderUpgradeMenu(const Player* player, const Hand* hand) = 0;
    virtual void renderMainMenu(bool hasSave) = 0;

    virtual void renderLevelStart(const Level* level) = 0;
    virtual void renderLevelComplete(const Level* level, int moves, int hp) = 0;
    virtual void renderGameComplete(int finalHp) = 0;

    virtual void renderGameOver(int moveCount) = 0;
    virtual void renderVictory(int moveCount, int playerHealth) = 0;

    virtual void clearScreen() = 0;
    virtual void pause(int milliseconds) = 0;
};







