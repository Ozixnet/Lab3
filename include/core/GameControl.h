#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H

#include <memory>
#include "LevelManager.h"

// === FORWARD DECLARATIONS ===
class GameAction;
class Player;
class Board;
class Hand;

class GameControl {
public:
    GameControl();
    
    // Запретить копирование
    GameControl(const GameControl&) = delete;
    GameControl& operator=(const GameControl&) = delete;
    
    // Разрешить перемещение (с явным определением в .cpp)
    GameControl(GameControl&&) noexcept;
    GameControl& operator=(GameControl&&) noexcept;

    // явный деструктор
    ~GameControl();

    // Главный игровой цикл
    void gameStart();

    // Геттер количества ходов
    int getCountMove() const noexcept { return countMove; }
    
    // === МЕТОДЫ ДЛЯ СОХРАНЕНИЯ/ЗАГРУЗКИ ===
    // Геттеры для сериализации
    const Player* getPlayer() const { return player.get(); }
    Player* getPlayer() { return player.get(); }
    const Board* getBoard() const { return board.get(); }
    Board* getBoard() { return board.get(); }
    const Hand* getHand() const { return spellHand.get(); }
    Hand* getHand() { return spellHand.get(); }
    const LevelManager& getLevelManager() const { return levelManager; }
    LevelManager& getLevelManager() { return levelManager; }
    
    // Сеттеры для десериализации
    void setPlayer(std::unique_ptr<Player> p) { player = std::move(p); }
    void setBoard(std::unique_ptr<Board> b) { board = std::move(b); }
    void setHand(std::unique_ptr<Hand> h) { spellHand = std::move(h); }
    void setCountMove(int moves) { countMove = moves; }

private:
    // Состояние игры
    std::unique_ptr<Player> player;
    std::unique_ptr<Board> board;
    std::unique_ptr<Hand> spellHand;
    
    LevelManager levelManager;  // Управление уровнями

    int countMove{1};
    bool isRunning{false};
    bool quitToMenu{false};
    bool playerTurnComplete{false};

    // === ЖИЗНЕННЫЙ ЦИКЛ ===
    void initGame();                    // Создать игрока и руку (один раз)
    void loadLevel(int levelIndex);     // Загрузить конкретный уровень
    void cleanup();

    // === ОСНОВНОЙ ЦИКЛ
    void processInput(const std::string& input);
    void updateGame();
    void render();

    // === ОБРАБОТКА ДЕЙСТВИЙ - ФАЗЫ ХОДА
    void playerTurn(std::unique_ptr<GameAction> action);
    
    void alliesTurn();      // Фаза союзников
    void enemiesTurn();     // Фаза врагов
    void towersTurn();      // Фаза башен
    void buildingsTurn();   // Фаза зданий

    // === ПРОВЕРКИ СОСТОЯНИЯ
    bool isVictory() const;
    bool isGameOver() const;
    bool shouldQuitToMenu() const;
    
    // === МЕНЮ УРОВНЕЙ
    void showLevelStart();      // Показать начало уровня
    void showLevelComplete();   // Показать завершение уровня
    void showGameComplete();    // Показать завершение всей игры
    void prepareForNextLevel(); // Подготовка к следующему уровню (восстановление HP, удаление карт)
    
    // === СОХРАНЕНИЕ/ЗАГРУЗКА
    bool showMainMenu();        // Главное меню (новая игра / загрузить), возвращает true если нужно выйти
    void handleSaveGame();      // Сохранить игру
    void handleLoadGame();      // Загрузить игру
};

#endif // GAME_CONTROL_H
