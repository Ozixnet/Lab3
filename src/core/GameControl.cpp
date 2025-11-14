#include "core/GameControl.h"

// ВАЖНО: Включить полные определения ДО определения деструктора и move-операторов
// Иначе unique_ptr не сможет корректно удалить объекты
#include "Board/Board.h"
#include "Entity/Player/Player.h"
#include "Entity/EntityManager.h"
#include "Magic/Hand.h"
#include "Actions/GameAction.h"
#include "UI/InputHandler.h"
#include "UI/MenuRenderer.h"
#include "Utils/Random.h"
#include "core/Level.h"
#include "core/GameSaveManager.h"
#include "core/GameExceptions.h"

#include <iostream>
#include <limits>
#include <string>
#include <algorithm>
#include <cctype>
#include <windows.h>
#include <ctime>
#include <cstdlib>

// Конструктор по умолчанию
GameControl::GameControl()
    : isRunning(false), quitToMenu(false), playerTurnComplete(false) {}

// Деструктор (должен быть ПОСЛЕ включения полных определений Player, Board, Hand)
GameControl::~GameControl() = default;

// Move-конструкторы (должны быть ПОСЛЕ включения полных определений)
GameControl::GameControl(GameControl&&) noexcept = default;
GameControl& GameControl::operator=(GameControl&&) noexcept = default;

void GameControl::initGame() {
    system("cls");

    // Создать игрока и руку заклинаний (один раз за игру!)
    player = std::make_unique<Player>(1000, 5);
    spellHand = std::make_unique<Hand>(5);

    std::cout << "Игрок создан!\n";
    player->displayInfo();
}

void GameControl::loadLevel(int /* levelIndex */) {
    Level* level = levelManager.getCurrentLevel();
    if (!level) {
        throw std::runtime_error("Level not found!");
    }
    
    // Создать Board нужного размера для уровня
    int size = level->getBoardSize();
    board = std::make_unique<Board>(size, *player);
    
    // Разместить игрока на поле
    board->getEntityManager().playerPosted();
    
    // Инициализировать уровень (враги/башни/здания)
    level->initialize(board->getEntityManager());
    
    // Добавить ловушки на поле (опционально)
    //board->addSlowTraps(3);
    
    // Сбросить счетчик ходов для нового уровня
    countMove = 1;
}

void GameControl::gameStart() {
    srand(static_cast<unsigned>(time(0)));
    std::cout << "Welcome to the game!\n";
    SetConsoleOutputCP(CP_UTF8);

    // === ЦИКЛ ГЛАВНОГО МЕНЮ ===
    while (true) {
        // Показать главное меню
        if (showMainMenu()) {
            // Пользователь выбрал выход
            return;
        }
        
        // === ГЛАВНОЕ МЕНЮ ===
        bool loadedFromSave = false;
        
        // Проверить, была ли загружена игра
        if (board && player && spellHand) {
            loadedFromSave = true;
        } else {
            // === СОЗДАТЬ ИГРОКА И РУКУ (один раз за игру) ===
            initGame();
            
            // === ЦИКЛ ПО УРОВНЯМ ===
            levelManager.reset();
            
            // Начать с первого уровня
            if (!levelManager.loadLevel(0)) {
                continue;  // Вернуться в главное меню, если нет уровней
            }
        }
        
        // === ИГРОВОЙ ЦИКЛ ===
        while (levelManager.getCurrentLevel()) {
            try {
                // === ЗАГРУЗИТЬ УРОВЕНЬ (только если не загружено из сохранения) ===
                if (!loadedFromSave) {
                    loadLevel(levelManager.getCurrentLevelIndex());
                    showLevelStart();
                } else {
                    // Игра загружена, просто показать информацию
                    system("cls");
                    std::cout << "\nИгра загружена. Продолжаем...\n";
                    std::cin.get();
                    loadedFromSave = false;  // Сбросить флаг
                }
                
                isRunning = true;
                quitToMenu = false;
                
                // === ИГРОВОЙ ЦИКЛ УРОВНЯ ===
                while (isRunning && !quitToMenu) {
                    try {
                        render();

                        std::string input;
                        std::cout << "Введите команду: ";
                        std::getline(std::cin, input);
                        
                        // Убрать пробелы в начале и конце
                        if (!input.empty()) {
                            input.erase(0, input.find_first_not_of(" \t\n\r"));
                            if (!input.empty()) {
                                input.erase(input.find_last_not_of(" \t\n\r") + 1);
                            }
                        }
                        
                        // Пропустить пустую строку
                        if (input.empty()) {
                            continue;
                        }

                        processInput(input);

                        if (playerTurnComplete) {
                            updateGame();
                            
                            // === ФАЗЫ ХОДА (В ПОРЯДКЕ!) ===
                            alliesTurn();      // 1. Союзники
                            enemiesTurn();     // 2. Враги
                            towersTurn();      // 3. Башни
                            buildingsTurn();   // 4. Здания
                            
                            playerTurnComplete = false;
                        }

                        // Проверка на поражение
                        if (isGameOver()) {
                            MenuRenderer::renderGameOver(countMove);
                            std::cout << "\nНачать заново игру или выйти? (y - начать заново, n - выйти): ";
                            char retry;
                            std::cin >> retry;
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            if (retry == 'y' || retry == 'Y') {
                                // Y - Полностью сбросить и начать с первого уровня
                                cleanup();  // Очистить board
                                player.reset();  // Удалить старого игрока
                                spellHand.reset();  // Удалить старую руку
                                levelManager.reset();  // Сбросить менеджер уровней
                                
                                // Создать нового игрока и руку (с нормальными параметрами)
                                player = std::make_unique<Player>(1000, 5);
                                spellHand = std::make_unique<Hand>(5);
                                
                                // Начать с первого уровня
                                if (levelManager.loadLevel(0)) {
                                    loadLevel(0);
                                    showLevelStart();
                                    continue;  // Продолжить с новым уровнем
                                } else {
                                    // Нет уровней, вернуться в меню
                                    isRunning = false;
                                    break;
                                }
                            } else {
                                // N - Выйти из игры (вернуться в главное меню)
                                cleanup();
                                player.reset();
                                spellHand.reset();
                                levelManager.reset();
                                isRunning = false;
                                quitToMenu = true;
                                break;
                            }
                        }

                        // Проверка на победу
                        if (isVictory()) {
                            showLevelComplete();
                            
                            // Есть следующий уровень?
                            if (levelManager.hasNextLevel()) {
                                prepareForNextLevel();  // Восстановить HP, удалить половину карт
                                levelManager.nextLevel();
                                break;  // Выйти из цикла уровня, загрузить следующий
                            } else {
                                showGameComplete();  // Все уровни пройдены!
                                isRunning = false;
                                break;
                            }
                        }
                        
                    } catch (const std::exception& e) {
                        std::cerr << "Error in game loop: " << e.what() << '\n';
                    } catch (...) {
                        std::cerr << "Unknown error in game loop\n";
                    }
                }
                
                if (!isRunning || quitToMenu) {
                    // Если quitToMenu - вернуться в главное меню
                    if (quitToMenu) {
                        quitToMenu = false;  // Сбросить флаг
                        cleanup();  // Очистить текущее состояние
                        break;  // Выйти из цикла уровней, вернуться в главное меню
                    }
                    break;  // Выйти из цикла уровней
                }
                
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << '\n';
                break;
            } catch (...) {
                std::cerr << "Unknown error\n";
                break;
            }
            
            // Если вышли из цикла уровней, вернуться в главное меню
            cleanup();
        }
    }
}

void GameControl::processInput(const std::string& input) {
    try {
        std::unique_ptr<GameAction> action;
        
        // Преобразовать в нижний регистр для сравнения
        std::string lowerInput = input;
        std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);

        // Выход
        if (lowerInput == "q" || lowerInput == "quit" || lowerInput == "exit") {
            quitToMenu = true;
            return;
        }
        
        // Сохранение игры
        if (lowerInput == "save") {
            handleSaveGame();
            return;
        }
        
        // Загрузка игры
        if (lowerInput == "load") {
            handleLoadGame();
            return;
        }

        // Движение (одна буква)
        if (input.length() == 1) {
            char cmd = input[0];
            if (cmd == 'w' || cmd == 'W' || 
                cmd == 'a' || cmd == 'A' || 
                cmd == 's' || cmd == 'S' ||
                cmd == 'd' || cmd == 'D') {
                action = InputHandler::handleMovementInput(cmd);
            }
            // Меню опций
            else if (cmd == 'o' || cmd == 'O') {
                action = InputHandler::handleOptionsMenu(board.get(), spellHand.get(), player.get());
            }
            else {
                std::cout << "Неизвестная команда: " << input << "\n";
                std::cout << "Используйте: w/a/s/d - движение, o - меню, save - сохранить, load - загрузить, q - выход\n";
                return;
            }
        }
        else {
            std::cout << "Неизвестная команда: " << input << "\n";
            std::cout << "Используйте: w/a/s/d - движение, o - меню, save - сохранить, load - загрузить, q - выход\n";
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
    Level* level = levelManager.getCurrentLevel();
    if (!level) return false;
    
    // Проверка через Level (гибкие условия победы!)
    return level->isVictoryCondition(board->getEntityManager());
}

void GameControl::render() {
    MenuRenderer::renderGameScreen(board.get(), player.get(), spellHand.get(), countMove);
}

void GameControl::updateGame() {
    countMove++;
}

// === ФАЗЫ ХОДА (РАЗДЕЛЕНО!) ===

void GameControl::alliesTurn() {
    // 1. Движение союзников
    board->getEntityManager().moveAllies();
    
    // TODO: Добавить атаки союзников (если рядом враги)
}

void GameControl::enemiesTurn() {
    // 1. Движение врагов
    board->getEntityManager().moveAllEnemies();
    
    // TODO: Добавить атаки врагов (если рядом игрок/союзники)
}

void GameControl::towersTurn() {
    // Башни уже обработаны в playerTurn (updateAllTowers и checkTowerAttack)
    // Здесь ничего не делаем
}

void GameControl::buildingsTurn() {
    // Спавн врагов из зданий
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
                playerTurnComplete = false;
            } catch (...) {
                std::cerr << "Unknown error in post-turn processing\n";
                playerTurnComplete = false;
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
    // НЕ удаляем player и spellHand - они сохраняются между уровнями!
    countMove = 1;
}

// === МЕНЮ УРОВНЕЙ ===

void GameControl::showLevelStart() {
    Level* level = levelManager.getCurrentLevel();
    if (!level) return;
    
    system("cls");
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════╗\n";
    std::cout << "║       " << level->getName() << "      ║\n";
    std::cout << "╠═══════════════════════════════════════╣\n";
    std::cout << "║  " << level->getDescription() << "    ║\n";
    std::cout << "╠═══════════════════════════════════════╣\n";
    std::cout << "║  Размер поля: " << level->getBoardSize() << "x" << level->getBoardSize() << "                 ║\n";
    std::cout << "║  Сложность: ";
    for (int i = 0; i < level->getDifficulty(); ++i) {
        std::cout << "★";
    }
    for (int i = level->getDifficulty(); i < 5; ++i) {
        std::cout << "☆";
    }
    std::cout << "                                        ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n";
    std::cout << "\nНажмите Enter для начала...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void GameControl::showLevelComplete() {
    Level* level = levelManager.getCurrentLevel();
    
    system("cls");
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════╗\n";
    std::cout << "║         УРОВЕНЬ ПРОЙДЕН! 🎉          ║\n";
    std::cout << "╠═══════════════════════════════════════╣\n";
    std::cout << "║  " << level->getName() << "  ║\n";
    std::cout << "║  Ходов: " << countMove << "                          ║\n";
    std::cout << "║  HP: " << player->GetHealth() << " / " << player->GetMaxHealth() << "                        ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n";
    
    if (levelManager.hasNextLevel()) {
        std::cout << "\nПереход к следующему уровню...\n";
    }
    
    std::cout << "Нажмите Enter для продолжения...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void GameControl::showGameComplete() {
    system("cls");
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════╗\n";
    std::cout << "║       ВЫ ПРОШЛИ ВСЮ ИГРУ! 🏆         ║\n";
    std::cout << "╠═══════════════════════════════════════╣\n";
    std::cout << "║  Все уровни пройдены!                ║\n";
    std::cout << "║  Финальный HP: " << player->GetHealth() << " / " << player->GetMaxHealth() << "            ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n";
    std::cout << "\nСпасибо за игру!\n";
    std::cout << "Нажмите Enter...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// === СОХРАНЕНИЕ/ЗАГРУЗКА ===

bool GameControl::showMainMenu() {
    system("cls");
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════╗\n";
    std::cout << "║         ГЛАВНОЕ МЕНЮ                  ║\n";
    std::cout << "╠═══════════════════════════════════════╣\n";
    std::cout << "║  1. Новая игра                      ║\n";
    
    if (GameSaveManager::saveExists()) {
        std::cout << "║  2. Загрузить игру                  ║\n";
    } else {
        std::cout << "║  2. Загрузить игру (нет сохранения) ║\n";
    }
    
    std::cout << "║  0 или Q. Выход                     ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n";
    std::cout << "\nВыберите действие: ";
    
    std::string input;
    std::getline(std::cin, input);
    
    // Убрать пробелы в начале и конце
    if (!input.empty()) {
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        if (!input.empty()) {
            input.erase(input.find_last_not_of(" \t\n\r") + 1);
        }
    }
    
    // Преобразовать в нижний регистр
    std::string lowerInput = input;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
    
    // Проверка на выход (Q или 0)
    if (lowerInput == "q" || lowerInput == "quit" || lowerInput == "exit" || lowerInput == "0") {
        return true;  // Выход из программы
    }
    
    // Попытка преобразовать в число
    int choice = 0;
    try {
        choice = std::stoi(input);
    } catch (...) {
        // Не число, игнорируем
    }
    
    if (choice == 2 && GameSaveManager::saveExists()) {
        try {
            handleLoadGame();
            // После загрузки игра продолжается
            return false;  // Продолжить игру
        } catch (const LoadException& e) {
            std::cout << "\n❌ Ошибка загрузки: " << e.what() << "\n";
            std::cout << "Нажмите Enter для возврата в меню...\n";
            std::cin.get();
            return false;  // Вернуться в меню
        } catch (const std::exception& e) {
            std::cout << "\n❌ Неожиданная ошибка: " << e.what() << "\n";
            std::cout << "Нажмите Enter для возврата в меню...\n";
            std::cin.get();
            return false;  // Вернуться в меню
        }
    }
    
    // choice == 1 или нет сохранения - продолжаем с новой игрой
    return false;  // Продолжить игру
}

void GameControl::handleSaveGame() {
    if (!board || !player || !spellHand) {
        std::cout << "❌ Невозможно сохранить: игра не инициализирована!\n";
        return;
    }
    
    try {
        GameSaveManager::saveGame(*this);
        std::cout << "\n✅ Игра успешно сохранена!\n";
        std::cout << "Нажмите Enter для продолжения...\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    } catch (const SaveException& e) {
        std::cout << "\n❌ Ошибка сохранения: " << e.what() << "\n";
        std::cout << "Нажмите Enter для продолжения...\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    } catch (const std::exception& e) {
        std::cout << "\n❌ Неожиданная ошибка при сохранении: " << e.what() << "\n";
        std::cout << "Нажмите Enter для продолжения...\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }
}

void GameControl::prepareForNextLevel() {
    if (!player || !spellHand) {
        return;
    }
    
    // Восстановить HP игрока
    player->RestoreHealth();
    std::cout << "\n✨ HP восстановлено до максимума!\n";
    
    // Добавить очки прокачки
    int upgradePointsGained = 3;  // 3 очка за каждый пройденный уровень
    player->AddUpgradePoints(upgradePointsGained);
    std::cout << "🌟 Получено очков прокачки: " << upgradePointsGained << "\n";
    std::cout << "💎 Всего очков прокачки: " << player->GetUpgradePoints() << "\n";
    
    // Удалить половину карт случайным образом
    size_t cardsBefore = spellHand->size();
    spellHand->removeHalfRandomly();
    size_t cardsAfter = spellHand->size();
    
    if (cardsBefore > cardsAfter) {
        std::cout << "💫 Половина карт заклинаний была утеряна! (" << (cardsBefore - cardsAfter) << " карт)\n";
    }
    
    std::cout << "\n🔧 Используйте меню опций (O) для прокачки персонажа!\n";
    std::cout << "Нажмите Enter для продолжения...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void GameControl::handleLoadGame() {
    if (!GameSaveManager::saveExists()) {
        std::cout << "\n❌ Сохранение не найдено!\n";
        std::cout << "Нажмите Enter...\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        return;
    }
    
    try {
        // Очистить текущее состояние (но сохранить player и spellHand для инициализации)
        board.reset();
        
        // Создать игрока и руку если их нет (для инициализации)
        if (!player) {
            player = std::make_unique<Player>(1000, 5);
        }
        if (!spellHand) {
            spellHand = std::make_unique<Hand>(5);
        }
        
        // Загрузить игру
        GameSaveManager::loadGame(*this);
        
        std::cout << "\n✅ Игра успешно загружена!\n";
        std::cout << "Нажмите Enter для продолжения...\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        
        // Установить флаги для продолжения игры
        isRunning = true;
        quitToMenu = false;
        
    } catch (const FileNotFoundException& e) {
        std::cout << "\n❌ Файл сохранения не найден: " << e.what() << "\n";
        std::cout << "Нажмите Enter...\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        throw;  // Пробросить дальше для обработки в showMainMenu
    } catch (const InvalidDataException& e) {
        std::cout << "\n❌ Файл сохранения поврежден: " << e.what() << "\n";
        std::cout << "Нажмите Enter...\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        throw;  // Пробросить дальше
    } catch (const LoadException& e) {
        std::cout << "\n❌ Ошибка загрузки: " << e.what() << "\n";
        std::cout << "Нажмите Enter...\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        throw;  // Пробросить дальше
    } catch (const std::exception& e) {
        std::cout << "\n❌ Неожиданная ошибка при загрузке: " << e.what() << "\n";
        std::cout << "Нажмите Enter...\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        throw;  // Пробросить дальше
    }
}

