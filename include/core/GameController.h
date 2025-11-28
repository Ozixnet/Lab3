#pragma once

#include <algorithm>
#include <cctype>
#include <ctime>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <windows.h>

#include "Actions/GameAction.h"
#include "Board/Board.h"
#include "Entity/Player/Player.h"
#include "Magic/Hand.h"
#include "Rendering/ConsoleRenderer.h"
#include "UI/GameView.h"
#include "Utils/Random.h"
#include "Input/GameCommand.h"
#include "core/GameExceptions.h"
#include "core/GameSaveManager.h"
#include "core/Level.h"
#include "core/LevelManager.h"
#include "Events/EventBus.h"
#include "Events/GameEvent.h"

template<typename, typename = void>
struct has_set_renderer : std::false_type {};

template<typename T>
struct has_set_renderer<T, std::void_t<decltype(std::declval<T&>().setRenderer(std::declval<IGameRenderer*>()))>> : std::true_type {};

/**
 * @brief Шаблонный класс управления игрой
 * 
 * @requirement Требование 3: Создать шаблонный класс управления игрой. 
 * В качестве параметра шаблона должен передаваться класс, отвечающий за считывание и преобразование ввода.
 * 
 * @tparam TInputReader Класс ввода (ConsoleInputReader, GuiInputReader, NetworkInputReader и т.д.)
 * @tparam TRenderer Класс отрисовки (ConsoleRenderer, ImprovedGuiRenderer и т.д.)
 * 
 * Создает объект TInputReader и получает от него команды, далее вызывает нужное действие у классов игры.
 * Данный класс не создает объект класса игры - только управляет игровым процессом.
 * 
 * Масштабируемость: можно реализовать получение команд через интернет без использования реализации интерфейса,
 * и просто подставить новый класс в качестве параметра шаблона.
 * 
 * @see IInputReader
 * @see GameView
 * @see GameAction
 */
template<typename TInputReader, typename TRenderer = ConsoleRenderer>
class GameController {
public:
    GameController();
    ~GameController() = default;

    GameController(const GameController&) = delete;
    GameController& operator=(const GameController&) = delete;
    GameController(GameController&&) noexcept = delete;
    GameController& operator=(GameController&&) noexcept = delete;

    void gameStart();

    const Player* getPlayer() const { return player.get(); }
    Player* getPlayer() { return player.get(); }
    const Board* getBoard() const { return board.get(); }
    Board* getBoard() { return board.get(); }
    const Hand* getHand() const { return spellHand.get(); }
    Hand* getHand() { return spellHand.get(); }
    const LevelManager& getLevelManager() const { return levelManager; }
    LevelManager& getLevelManager() { return levelManager; }
    int getCountMove() const noexcept { return countMove; }

    void setPlayer(std::unique_ptr<Player> p) { player = std::move(p); }
    void setBoard(std::unique_ptr<Board> b) { board = std::move(b); }
    void setHand(std::unique_ptr<Hand> h) { spellHand = std::move(h); }
    void setCountMove(int moves) { countMove = moves; }

    GameView<TRenderer>& getGameView() { return gameView; }
    const GameView<TRenderer>& getGameView() const { return gameView; }
    TInputReader& getInputReader() { return inputReader; }
    const TInputReader& getInputReader() const { return inputReader; }

private:
    void initGame();
    void loadLevel(int levelIndex);
    void cleanup();

    void runGameLoop();
    void runLevelLoop();

    void processInput();
    void updateGame();
    void render();

    void playerTurn(std::unique_ptr<GameAction> action);
    void alliesTurn();
    void enemiesTurn();
    void towersTurn();
    void buildingsTurn();

    bool isVictory() const;
    bool isGameOver() const;

    bool showMainMenu();
    void showLevelStart();
    void showLevelComplete();
    void showGameComplete();

    void handleSaveGame();
    void handleLoadGame(bool resumeFromMenu = false);
    void prepareForNextLevel();

private:
    TInputReader inputReader;
    GameView<TRenderer> gameView;

    std::unique_ptr<Player> player;
    std::unique_ptr<Board> board;
    std::unique_ptr<Hand> spellHand;
    LevelManager levelManager;

    bool isRunning{false};
    bool quitToMenu{false};
    bool playerTurnComplete{false};
    bool loadedFromSave{false};
    int countMove{1};
};

template<typename TInputReader, typename TRenderer>
GameController<TInputReader, TRenderer>::GameController() {
    if constexpr (has_set_renderer<TInputReader>::value) {
        inputReader.setRenderer(gameView.getRendererInterface());
    }
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::gameStart() {
    srand(static_cast<unsigned>(time(0)));
    SetConsoleOutputCP(CP_UTF8);

    while (true) {
        if (showMainMenu()) {
            return;
        }

        if (!loadedFromSave) {
            initGame();
            levelManager.reset();
            if (!levelManager.loadLevel(0)) {
                continue;
            }
        }

        runGameLoop();
        cleanup();
        loadedFromSave = false;
    }
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::runGameLoop() {
    while (levelManager.getCurrentLevel()) {
        try {
            if (loadedFromSave) {
                gameView.clearScreen();
                std::cout << "\nИгра загружена. Продолжаем...\n";
                if (Level* level = levelManager.getCurrentLevel()) {
                    EventBus::getInstance().publish(LevelStartedEvent(level->getName(), levelManager.getCurrentLevelIndex()));
                }
                // Ждём любую клавишу через inputReader (не блокирует консоль в GUI режиме)
                inputReader.readRawInput();
                loadedFromSave = false;
            } else {
                loadLevel(levelManager.getCurrentLevelIndex());
                showLevelStart();
            }

            runLevelLoop();

            if (quitToMenu) {
                quitToMenu = false;
                break;
            }

            if (!isRunning) {
                break;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << '\n';
            break;
        }
    }
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::runLevelLoop() {
    isRunning = true;
    quitToMenu = false;

    while (isRunning && !quitToMenu) {
        try {
            render();
            processInput();

            if (playerTurnComplete) {
                updateGame();
                alliesTurn();
                enemiesTurn();
                towersTurn();
                buildingsTurn();
                playerTurnComplete = false;
            }

            if (isGameOver()) {
                EventBus::getInstance().publish(GameOverEvent(countMove));
                gameView.onGameOver(countMove);
                
                // Просто выходим в главное меню
                isRunning = false;
                quitToMenu = true;
                break;
            }

            if (isVictory()) {
                showLevelComplete();
                if (levelManager.hasNextLevel()) {
                    prepareForNextLevel();
                    levelManager.nextLevel();
                    break;
                } else {
                    showGameComplete();
                    isRunning = false;
                    break;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in game loop: " << e.what() << '\n';
        }
    }
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::initGame() {
    gameView.clearScreen();
    player = std::make_unique<Player>(1000, 5);
    spellHand = std::make_unique<Hand>(5);
    std::cout << "Игрок создан!\n";
    player->displayInfo();
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::loadLevel(int /* levelIndex */) {
    Level* level = levelManager.getCurrentLevel();
    if (!level) {
        throw std::runtime_error("Level not found!");
    }

    int size = level->getBoardSize();
    board = std::make_unique<Board>(size, *player);
    board->getEntityManager().playerPosted();
    level->initialize(board->getEntityManager());
    countMove = 1;
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::cleanup() {
    board.reset();
    countMove = 1;
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::processInput() {
    std::string input = inputReader.readRawInput();
    if (input.empty()) {
        return;
    }

    GameCommand command = inputReader.mapInputToCommand(input);
    switch (command) {
        case GameCommand::QUIT:
            quitToMenu = true;
            return;
        case GameCommand::SAVE:
            handleSaveGame();
            return;
        case GameCommand::LOAD:
            handleLoadGame(false);
            return;
        default:
            break;
    }

    auto action = inputReader.parseCommand(input, board.get(), spellHand.get(), player.get());
    if (action) {
        playerTurn(std::move(action));
    }
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::render() {
    gameView.onGameStateChanged(board.get(), player.get(), spellHand.get(), countMove);
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::updateGame() {
    countMove++;
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::playerTurn(std::unique_ptr<GameAction> action) {
    try {
        if (!action) {
            std::cout << "[WARNING] playerTurn: action is null\n";
            return;
        }

        int result = action->execute(*board, *player);

        if (result == 0 || result == 1 || result == 3 || result == 4) {
            playerTurnComplete = true;

            try {
                board->getEntityManager().updateAllTowers();
                auto [playerX, playerY] = board->getEntityManager().getPlayerCoord();
                board->getEntityManager().checkTowerAttack(playerX, playerY);

                int trapDamage = board->getEntityManager().checkTrapDamage(playerX, playerY);
                if (trapDamage > 0) {
                    player->ReduceHealth(trapDamage);
                    std::cout << "💥 Вы наступили на ловушку! Получили " << trapDamage << " урона!\n";
                }

                if (result == 1 && spellHand && spellHand->size() < 5) {
                    if (Random::chance(20)) {
                        spellHand->addRandomSpell();
                        std::cout << "✨ Вы получили новое заклинание!\n";
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error in post-turn processing: " << e.what() << '\n';
                playerTurnComplete = false;
            } catch (...) {
                std::cerr << "Unknown error in post-turn processing\n";
                playerTurnComplete = false;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in playerTurn: " << e.what() << '\n';
        playerTurnComplete = false;
    } catch (...) {
        std::cerr << "Unknown error in playerTurn\n";
        playerTurnComplete = false;
    }
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::alliesTurn() {
    board->getEntityManager().moveAllies();
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::enemiesTurn() {
    board->getEntityManager().moveAllEnemies();
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::towersTurn() {
    // Башни обновляются в playerTurn
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::buildingsTurn() {
    board->getEntityManager().processBuildingSpawns();
}

template<typename TInputReader, typename TRenderer>
bool GameController<TInputReader, TRenderer>::isVictory() const {
    Level* level = levelManager.getCurrentLevel();
    if (!level) return false;
    return level->isVictoryCondition(board->getEntityManager());
}

template<typename TInputReader, typename TRenderer>
bool GameController<TInputReader, TRenderer>::isGameOver() const {
    return player && player->GetHealth() <= 0;
}

template<typename TInputReader, typename TRenderer>
bool GameController<TInputReader, TRenderer>::showMainMenu() {
    bool hasSave = GameSaveManager::saveExists();
    gameView.showMainMenu(hasSave);

    std::string input = inputReader.readRawInput();
    if (input.empty()) {
        return false;
    }

    std::string lowerInput = input;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);

    if (lowerInput == "q" || lowerInput == "quit" || lowerInput == "exit" || lowerInput == "0") {
        return true;
    }

    if (lowerInput == "2" && hasSave) {
        try {
            handleLoadGame(true);
            return false;
        } catch (const std::exception& e) {
            std::cout << "\n❌ Ошибка загрузки: " << e.what() << "\n";
            // Не блокируем, просто возвращаемся в меню
            return false;
        }
    }

    return false;
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::showLevelStart() {
    Level* level = levelManager.getCurrentLevel();
    if (!level) return;

    gameView.onLevelStart(level);
    EventBus::getInstance().publish(LevelStartedEvent(level->getName(), levelManager.getCurrentLevelIndex()));
    // Ждём клавишу через inputReader
    inputReader.readRawInput();
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::showLevelComplete() {
    Level* level = levelManager.getCurrentLevel();
    gameView.onLevelComplete(level, countMove, player ? player->GetHealth() : 0);
    if (level) {
        EventBus::getInstance().publish(LevelCompletedEvent(level->getName(), levelManager.getCurrentLevelIndex(), countMove, player ? player->GetHealth() : 0));
    }
    // Ждём клавишу через inputReader
    inputReader.readRawInput();
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::showGameComplete() {
    gameView.onGameComplete(player ? player->GetHealth() : 0);
    // Ждём клавишу через inputReader
    inputReader.readRawInput();
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::handleSaveGame() {
    if (!board || !player || !spellHand) {
        std::cout << "❌ Невозможно сохранить: игра не инициализирована!\n";
        return;
    }

    try {
        GameSaveManager::saveGame(*this);
        std::cout << "\n✅ Игра успешно сохранена!\n";
        // Не блокируем, просто продолжаем
    } catch (const std::exception& e) {
        std::cout << "\n❌ Ошибка сохранения: " << e.what() << "\n";
        // Не блокируем, просто продолжаем
    }
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::handleLoadGame(bool resumeFromMenu) {
    try {
        GameSaveManager::loadGame(*this);
        if (resumeFromMenu) {
            loadedFromSave = true;
        }
        std::cout << "\n✅ Игра успешно загружена!\n";
        // Не блокируем, просто продолжаем
    } catch (const std::exception& e) {
        std::cout << "\n❌ Ошибка загрузки: " << e.what() << "\n";
        // Не блокируем, просто продолжаем
    }
}

template<typename TInputReader, typename TRenderer>
void GameController<TInputReader, TRenderer>::prepareForNextLevel() {
    if (!player || !spellHand) {
        return;
    }

    player->RestoreHealth();
    std::cout << "\n✨ HP восстановлено до максимума!\n";
    
    // 🎯 Бонусные очки за прохождение уровня!
    int bonusPoints = 2;
    player->AddUpgradePoints(bonusPoints);
    std::cout << "⭐ +" << bonusPoints << " очков прокачки за прохождение уровня!\n";

    if (spellHand->size() > 2) {
        while (spellHand->size() > 2) {
            spellHand->removeSpell(spellHand->size() - 1);
        }
        std::cout << "🃏 Лишние карты удалены для баланса!\n";
    }
}


