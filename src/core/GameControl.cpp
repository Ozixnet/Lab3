#include "Core/GameControl.h"

#include "Board/Board.h"
#include "Entity/Player/Player.h"
#include "Magic/Hand.h"
#include "Actions/GameAction.h"
#include "UI/InputHandler.h"
#include "UI/MenuRenderer.h"
#include "Utils/Random.h"

#include <iostream>
#include <windows.h>
#include <ctime>
#include <cstdlib>

GameControl::~GameControl() = default;

GameControl::GameControl()
    : isRunning(false), quitToMenu(false), playerTurnComplete(false) {}

void GameControl::initGame(int boardSize) {
    system("cls");

    player = std::make_unique<Player>(15, 5);
    board = std::make_unique<Board>(boardSize, *player);
    spellHand = std::make_unique<Hand>(5);

    board->getEntityManager().playerPosted();
    board->getEntityManager().addEnemy(3, 3, 2, 1);
    board->getEntityManager().addEnemyBuilding(7, 7, 10);
    board->getEntityManager().addEnemyTower(5, 5, 10);
    board->addSlowTraps(5);

    isRunning = true;
    quitToMenu = false;
    playerTurnComplete = false;

    std::cout << "Игрок создан!\n";
    player->displayInfo();
}

void GameControl::gameStart() {
    srand(static_cast<unsigned>(time(0)));
    std::cout << "Welcome to the game!\n";
    SetConsoleOutputCP(CP_UTF8);

    while (true) {
        try {
            int size = 0;
            std::cout << "Enter board size (10..25) or 0 to quit: ";

            if (!(std::cin >> size) || size == 0) {
                break;
            }

            try {
                initGame(size);

                while (isRunning && !quitToMenu) {
                    try {
                        render();

                        char input;
                        std::cout << "Введите команду: ";
                        std::cin >> input;

                        processInput(input);

                        if (playerTurnComplete) {
                            updateGame();
                            enemyTurn();  // Ход врагов и союзников после хода игрока
                            playerTurnComplete = false;
                        }

                        // Проверка на поражение
                        if (isGameOver()) {
                            MenuRenderer::renderGameOver(countMove);
                            isRunning = false;
                        }

                        // Проверка на победу
                        if (isVictory()) {
                            MenuRenderer::renderVictory(countMove, player->GetHealth());
                            isRunning = false;
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error in game loop: " << e.what() << '\n';
                        // Продолжаем работу, не падаем
                    } catch (...) {
                        std::cerr << "Unknown error in game loop\n";
                        // Продолжаем работу, не падаем
                    }
                }

                cleanup();

            } catch (const std::invalid_argument& e) {
                std::cerr << "Size error: " << e.what() << '\n';
                isRunning = false;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << '\n';
                isRunning = false;
            } catch (...) {
                std::cerr << "Unknown error\n";
                isRunning = false;
            }
        } catch (...) {
            std::cerr << "Critical error in gameStart\n";
            break;
        }
    }
}

void GameControl::processInput(char input) {
    try {
        std::unique_ptr<GameAction> action;

        // Выход
        if (input == 'q' || input == 'Q') {
            quitToMenu = true;
            return;
        }

        // Движение
        if (input == 'w' || input == 'W' || input == 's' || input == 'S' ||
            input == 'a' || input == 'A' || input == 'd' || input == 'D') {
            action = InputHandler::handleMovementInput(input);
        }
        // Меню опций
        else if (input == 'o' || input == 'O') {
            action = InputHandler::handleOptionsMenu(board.get(), spellHand.get());
        }
        else {
            std::cout << "Неизвестная команда!\n";
            return;
        }

        if (action) {
            playerTurn(std::move(action));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in processInput: " << e.what() << '\n';
        // Продолжаем прием ввода
    } catch (...) {
        std::cerr << "Unknown error in processInput\n";
        // Продолжаем прием ввода
    }
}

bool GameControl::isVictory() const {
    int aliveEnemies = board->getEntityManager().getAliveEnemyCount();
    int activeBuildings = board->getEntityManager().getActiveBuildingCount();
    return (aliveEnemies == 0 && activeBuildings == 0);
}

void GameControl::render() {
    MenuRenderer::renderGameScreen(board.get(), player.get(), spellHand.get(), countMove);
}

void GameControl::updateGame() {
    countMove++;
}

void GameControl::enemyTurn() {
    board->getEntityManager().moveAllEnemies();
    board->getEntityManager().moveAllies();  // Движение союзников
    board->getEntityManager().processBuildingSpawns();
}

void GameControl::playerTurn(std::unique_ptr<GameAction> action) {
    try {
        if (!action) {
            std::cout << "[WARNING] playerTurn: action is null\n";
            return;
        }

        int result = action->execute(*board, *player);

        if (result == 0 || result == 1 || result == 3 || result == 4) {
            playerTurnComplete = true;

            try {
                // === ОБНОВИТЬ БАШНИ ===
                board->getEntityManager().updateAllTowers();

                // === ПРОВЕРИТЬ АТАКУ БАШЕН ===
                // Башни атакуют через spell->use в tryAttack, урон наносится напрямую
                auto [playerX, playerY] = board->getEntityManager().getPlayerCoord();
                board->getEntityManager().checkTowerAttack(playerX, playerY);

                // === ПРОВЕРИТЬ ЛОВУШКИ ===
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
                playerTurnComplete = false;  // Отменяем завершение хода
            } catch (...) {
                std::cerr << "Unknown error in post-turn processing\n";
                playerTurnComplete = false;  // Отменяем завершение хода
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in playerTurn: " << e.what() << '\n';
        playerTurnComplete = false;  // Отменяем завершение хода
    } catch (...) {
        std::cerr << "Unknown error in playerTurn\n";
        playerTurnComplete = false;  // Отменяем завершение хода
    }
}

bool GameControl::isGameOver() const {
    return player->GetHealth() <= 0;
}

bool GameControl::shouldQuitToMenu() const {
    return quitToMenu;
}

void GameControl::cleanup() {
    board.reset();
    player.reset();
    spellHand.reset();
    countMove = 1;
}

