#pragma once

#include <memory>
#include <type_traits>

#include "Rendering/IGameRenderer.h"


class Board;
class Player;
class Hand;
class Level;

/**
 * @brief Шаблонный класс визуализации игры
 * 
 * @requirement Требование 4: Создать шаблонный класс визуализации игры.
 * В качестве параметра шаблона должен передаваться класс, отвечающий за способ отрисовки игры.
 * 
 * @tparam TRenderer Класс отрисовки (ConsoleRenderer, ImprovedGuiRenderer, WebRenderer и т.д.)
 * 
 * Данный класс создает объект класса отрисовки игры, и реагирует на изменения в игре,
 * и вызывает команду отрисовку.
 * 
 * Масштабируемость: можно реализовать отрисовку в виде веб-страницы без использования реализации интерфейса,
 * и просто подставить новый класс в качестве параметра шаблона.
 * 
 * @see IGameRenderer
 * @see GameController
 */
template<typename TRenderer>
class GameView {
public:
    GameView() = default;
    ~GameView() = default;

    void onGameStateChanged(const Board* board, const Player* player,
                            const Hand* hand, int moveCount) {
        renderer.renderGameScreen(board, player, hand, moveCount);
    }

    void onLevelStart(const Level* level) {
        renderer.renderLevelStart(level);
    }

    void onLevelComplete(const Level* level, int moves, int hp) {
        renderer.renderLevelComplete(level, moves, hp);
    }

    void onGameOver(int moveCount) {
        renderer.renderGameOver(moveCount);
    }

    void onVictory(int moveCount, int hp) {
        renderer.renderVictory(moveCount, hp);
    }

    void onGameComplete(int finalHp) {
        renderer.renderGameComplete(finalHp);
    }

    void showOptionsMenu() {
        renderer.renderOptionsMenu();
    }

    void showUpgradeMenu(const Player* player, const Hand* hand) {
        renderer.renderUpgradeMenu(player, hand);
    }

    void showMainMenu(bool hasSave) {
        renderer.renderMainMenu(hasSave);
    }

    void clearScreen() {
        renderer.clearScreen();
    }

    void pause(int milliseconds) {
        renderer.pause(milliseconds);
    }

    TRenderer& getRenderer() { return renderer; }
    const TRenderer& getRenderer() const { return renderer; }

    IGameRenderer* getRendererInterface() {
        static_assert(std::is_base_of_v<IGameRenderer, TRenderer>,
                      "TRenderer must implement IGameRenderer");
        return &renderer;
    }

    const IGameRenderer* getRendererInterface() const {
        static_assert(std::is_base_of_v<IGameRenderer, TRenderer>,
                      "TRenderer must implement IGameRenderer");
        return &renderer;
    }

private:
    TRenderer renderer;
};

