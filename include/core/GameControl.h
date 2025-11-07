#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H

#include <memory>

// === FORWARD DECLARATIONS ===
class GameAction;
class Player;
class Board;
class Hand;

class GameControl {
public:
    GameControl();

    // === ЯВНЫЙ ДЕСТРУКТОР (ключ!) ===
    ~GameControl();

    // Главный игровой цикл
    void gameStart();

    // Геттер количества ходов
    int getCountMove() const noexcept { return countMove; }

private:
    // Состояние игры
    std::unique_ptr<Player> player;
    std::unique_ptr<Board> board;
    std::unique_ptr<Hand> spellHand;

    int countMove{1};
    bool isRunning{false};
    bool quitToMenu{false};
    bool playerTurnComplete{false};

    // === ЖИЗНЕННЫЙ ЦИКЛ ===
    void initGame(int boardSize);
    void cleanup();

    // === ОСНОВНОЙ ЦИКЛ ===
    void processInput(char input);
    void updateGame();
    void render();

    // === ОБРАБОТКА ДЕЙСТВИЙ ===
    void playerTurn(std::unique_ptr<GameAction> action);
    void enemyTurn();

    // === ПРОВЕРКИ СОСТОЯНИЯ ===
    bool isVictory() const;
    bool isGameOver() const;
    bool shouldQuitToMenu() const;
};

#endif // GAME_CONTROL_H
