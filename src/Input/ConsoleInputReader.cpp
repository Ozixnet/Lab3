#include "Input/ConsoleInputReader.h"
#include "Input/Direction.h"

#include "Actions/AttackAction.h"
#include "Actions/MoveAction.h"
#include "Actions/SpellAction.h"
#include "Actions/ToggleRangeAction.h"
#include "Board/Board.h"
#include "Entity/Player/Player.h"
#include "Magic/Hand.h"
#include "Magic/SpellCard.h"
#include "Rendering/ConsoleRenderer.h"
#include "Rendering/IGameRenderer.h"
#include "UI/ConsoleUtils.h"
#include "Utils/Random.h"

#include <algorithm>
#include <conio.h>
#include <cctype>
#include <iostream>
#include <vector>

ConsoleInputReader::ConsoleInputReader() {
    std::cout << "\n[DEBUG ConsoleInputReader] Инициализация...\n";

    std::vector<std::string> possiblePaths = {
        "config/keybindings.cfg",           // Относительно текущей директории
        "../config/keybindings.cfg",        // Если запускаем из cmake-build-debug
        "../../config/keybindings.cfg",     // Если запускаем из подпапки
        "keybindings.cfg",                   // Прямо в текущей директории
        "C:/Users/Arkana/CLionProjects/untitled2/config/keybindings.cfg"
    };

    bool loaded = false;
    std::string loadedPath;
    for (const auto& path : possiblePaths) {
        std::cout << "[DEBUG] Пробую загрузить из: " << path << "\n";
        loaded = keyBindings.loadFromFile(path);
        if (loaded) {
            loadedPath = path;
            std::cout << "[DEBUG] ✓ Конфигурация успешно загружена из: " << path << "\n";
            break;
        }
    }

    if (loaded) {
        // Выводим текущие привязки
        std::cout << "\n";
        std::cout << "═══════════════════════════════════════\n";
        std::cout << "✅ КОНФИГУРАЦИЯ ЗАГРУЖЕНА УСПЕШНО!\n";
        std::cout << "═══════════════════════════════════════\n";
        keyBindings.printBindings();
        std::cout << "═══════════════════════════════════════\n\n";
    } else {
        std::cerr << "\n";
        std::cerr << "═══════════════════════════════════════\n";
        std::cerr << "⚠️ КОНФИГУРАЦИЯ НЕ ЗАГРУЖЕНА!\n";
        std::cerr << "   Используются дефолтные настройки (WASD)\n";
        std::cerr << "   Проверьте наличие файла config/keybindings.cfg\n";
        std::cerr << "═══════════════════════════════════════\n\n";
    }
}

void ConsoleInputReader::setRenderer(IGameRenderer* rendererPtr) {
    renderer = rendererPtr;
    std::cout << "[DEBUG ConsoleInputReader] setRenderer вызван\n";

    // Устанавливаем KeyBindings в ConsoleRenderer для отображения правильных подсказок
    if (rendererPtr) {
        ConsoleRenderer* consoleRenderer = dynamic_cast<ConsoleRenderer*>(rendererPtr);
        if (consoleRenderer) {
            consoleRenderer->setKeyBindings(&keyBindings);
            std::cout << "[DEBUG] ✓ KeyBindings установлен в ConsoleRenderer\n";
        } else {
            std::cout << "[DEBUG] ✗ rendererPtr не является ConsoleRenderer (возможно GUI режим)\n";
        }
    } else {
        std::cout << "[DEBUG] ✗ rendererPtr == nullptr\n";
    }
}

GameCommand ConsoleInputReader::mapInputToCommand(const std::string& input) const {
    return keyBindings.getCommand(input);
}

std::string ConsoleInputReader::readRawInput() {
    std::string input;
    std::cout << "Введите команду: ";
    std::getline(std::cin, input);

    if (!input.empty()) {
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        if (!input.empty()) {
            input.erase(input.find_last_not_of(" \t\n\r") + 1);
        }
    }

    return input;
}

std::unique_ptr<GameAction> ConsoleInputReader::parseCommand(
    const std::string& input,
    Board* board,
    Hand* hand,
    Player* player
) {
    if (input.empty()) {
        return nullptr;
    }

    std::cout << "[DEBUG parseCommand] Входной ввод: '" << input << "'\n";
    GameCommand cmd = keyBindings.getCommand(input);
    std::cout << "[DEBUG parseCommand] Команда из KeyBindings: " << static_cast<int>(cmd) << "\n";

    switch (cmd) {

        // KeyBindings определил команду — передаём Direction (абстракция вместо символов)
        case GameCommand::MOVE_UP:
            return handleMovementInput(Direction::UP);
        case GameCommand::MOVE_DOWN:
            return handleMovementInput(Direction::DOWN);
        case GameCommand::MOVE_LEFT:
            return handleMovementInput(Direction::LEFT);
        case GameCommand::MOVE_RIGHT:
            return handleMovementInput(Direction::RIGHT);
        case GameCommand::OPEN_MENU:
            return handleOptionsMenu(board, hand, player);
        case GameCommand::QUIT:
        case GameCommand::SAVE:
        case GameCommand::LOAD:
            return nullptr;
        case GameCommand::NONE:
        default:
            break;
    }

    std::cout << "Неизвестная команда: " << input << "\n";
    return nullptr;
}

std::unique_ptr<GameAction> ConsoleInputReader::handleMovementInput(Direction direction) {
    return std::make_unique<MoveAction>(direction);
}

std::unique_ptr<GameAction> ConsoleInputReader::handleAttackInput(Board* board) {
    if (!board) {
        std::cout << "Ошибка: доска не инициализирована!\n";
        return nullptr;
    }

    std::cout << "\n";
    std::cout << "╔════════════════════════════════╗\n";
    std::cout << "║  Куда атакуем? WASD + Enter    ║\n";
    std::cout << "╚════════════════════════════════╝\n\n";
    board->displayBoard();

    char direction = getDirectionFromUser();
    if (direction == ' ') {
        std::cout << "Атака отменена.\n";
        return nullptr;
    }

    return std::make_unique<AttackAction>(direction);
}

std::unique_ptr<GameAction> ConsoleInputReader::handleSpellInput(Hand* hand, Board* board) {
    if (!hand || hand->isEmpty()) {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════╗\n";
        std::cout << "║  ✗ Нет доступных заклинаний    ║\n";
        std::cout << "╚════════════════════════════════╝\n";
        if (renderer) {
            renderer->pause(1500);
        } else {
            ConsoleUtils::pause(1500);
        }
        return nullptr;
    }

    if (!board) {
        std::cout << "Ошибка: доска не инициализирована!\n";
        return nullptr;
    }

    if (renderer) {
        renderer->clearScreen();
    } else {
        ConsoleUtils::clearScreen();
    }

    std::cout << "═══════════════════════════════════════\n";
    std::cout << "        ВЫБОР ЗАКЛИНАНИЯ               \n";
    std::cout << "═══════════════════════════════════════\n\n";

    board->displayBoard();

    std::cout << "\n";
    hand->showHand();

    int choice = getSpellChoice(hand);
    if (choice == -1) {
        std::cout << "Отмена использования заклинания.\n";
        if (renderer) {
            renderer->pause(1000);
        } else {
            ConsoleUtils::pause(1000);
        }
        return nullptr;
    }

    SpellCard* spell = hand->getSpell(choice);
    if (!spell) {
        std::cout << "Некорректный выбор заклинания!\n";
        if (renderer) {
            renderer->pause(1500);
        } else {
            ConsoleUtils::pause(1500);
        }
        return nullptr;
    }

    std::cout << "\n╔════════════════════════════════╗\n";
    std::cout << "║  Использование: " << spell->getName() << "\n";
    std::cout << "╚════════════════════════════════╝\n";

    return std::make_unique<SpellAction>(
        spell,
        &board->getEntityManager(),
        hand,
        board->getSize(),
        choice
    );
}

std::unique_ptr<GameAction> ConsoleInputReader::handleOptionsMenu(Board* board, Hand* hand, Player* player) {
    if (renderer) {
        renderer->clearScreen();
        renderer->renderOptionsMenu();
    } else {
        ConsoleUtils::clearScreen();
        renderOptionsMenuFallback();
    }

    char option = _getch();

    switch (option) {
        case 'a': case 'A':
            return handleAttackInput(board);
        case 'd': case 'D':
            return std::make_unique<ToggleRangeAction>();
        case 's': case 'S':
            return handleSpellInput(hand, board);
        case 'u': case 'U':
            return handleUpgradeMenu(player, hand);
        case 27:
            std::cout << "Отменено\n";
            return nullptr;
        default:
            std::cout << "Неизвестная опция\n";
            if (renderer) {
                renderer->pause(1000);
            } else {
                ConsoleUtils::pause(1000);
            }
            return nullptr;
    }
}

std::unique_ptr<GameAction> ConsoleInputReader::handleUpgradeMenu(Player* player, Hand* hand) {
    if (!player) {
        std::cout << "Ошибка: игрок не инициализирован!\n";
        if (renderer) {
            renderer->pause(1500);
        } else {
            ConsoleUtils::pause(1500);
        }
        return nullptr;
    }

    while (true) {
        if (renderer) {
            renderer->clearScreen();
            renderer->renderUpgradeMenu(player, hand);
        } else {
            ConsoleUtils::clearScreen();
            renderUpgradeMenuFallback(player, hand);
        }

        char option = _getch();

        switch (option) {
            case '1': {
                if (player->SpendUpgradePoint()) {
                    player->UpgradeMaxHealth(100);
                    std::cout << "\n✅ Максимальное HP увеличено на 100!\n";
                    std::cout << "Новое значение: " << player->GetMaxHealth() << "\n";
                    if (renderer) {
                        renderer->pause(1500);
                    } else {
                        ConsoleUtils::pause(1500);
                    }
                } else {
                    std::cout << "\n❌ Недостаточно очков прокачки!\n";
                    if (renderer) {
                        renderer->pause(1500);
                    } else {
                        ConsoleUtils::pause(1500);
                    }
                }
                break;
            }
            case '2': {
                if (player->SpendUpgradePoint()) {
                    player->UpgradeBaseDamage(2);
                    std::cout << "\n✅ Базовый урон увеличен на 2!\n";
                    std::cout << "Новое значение: " << player->GetBaseDamage() << "\n";
                    if (renderer) {
                        renderer->pause(1500);
                    } else {
                        ConsoleUtils::pause(1500);
                    }
                } else {
                    std::cout << "\n❌ Недостаточно очков прокачки!\n";
                    if (renderer) {
                        renderer->pause(1500);
                    } else {
                        ConsoleUtils::pause(1500);
                    }
                }
                break;
            }
            case '3': {
                if (player->GetUpgradePoints() < 2) {
                    std::cout << "\n❌ Требуется 2 очка прокачки!\n";
                    if (renderer) {
                        renderer->pause(1500);
                    } else {
                        ConsoleUtils::pause(1500);
                    }
                    break;
                }

                if (!hand || hand->isEmpty()) {
                    std::cout << "\n❌ Нет заклинаний для улучшения!\n";
                    if (renderer) {
                        renderer->pause(1500);
                    } else {
                        ConsoleUtils::pause(1500);
                    }
                    break;
                }

                int randomIndex = Random::getRange(0, hand->size() - 1);
                SpellCard* spell = hand->getSpell(randomIndex);

                if (spell) {
                    player->SpendUpgradePoint();
                    player->SpendUpgradePoint();

                    std::cout << "\n✅ Заклинание '" << spell->getName() << "' улучшено!\n";
                    std::cout << "Урон и радиус увеличены на 50%!\n";

                    if (renderer) {
                        renderer->pause(2000);
                    } else {
                        ConsoleUtils::pause(2000);
                    }
                } else {
                    std::cout << "\n❌ Ошибка при улучшении заклинания!\n";
                    if (renderer) {
                        renderer->pause(1500);
                    } else {
                        ConsoleUtils::pause(1500);
                    }
                }
                break;
            }
            case 27:
                std::cout << "\nВозврат в меню опций...\n";
                if (renderer) {
                    renderer->pause(500);
                } else {
                    ConsoleUtils::pause(500);
                }
                return nullptr;
            default:
                std::cout << "\n❌ Неизвестная опция!\n";
                if (renderer) {
                    renderer->pause(1000);
                } else {
                    ConsoleUtils::pause(1000);
                }
                break;
        }
    }
}

char ConsoleInputReader::getDirectionFromUser() {
    char direction = ' ';

    while (true) {
        if (direction != ' ') {
            std::cout << "\r                         \r";
            switch (direction) {
                case 'w': case 'W': std::cout << "↑ (вверх)"; break;
                case 'a': case 'A': std::cout << "← (влево)"; break;
                case 's': case 'S': std::cout << "↓ (вниз)"; break;
                case 'd': case 'D': std::cout << "→ (вправо)"; break;
            }
            std::cout.flush();
        }

        char key = _getch();

        if (key == '\r') {
            if (direction == ' ') {
                std::cout << "\nВыберите направление!\n";
                continue;
            }
            break;
        }

        if (key == 'w' || key == 'W' || key == 'a' || key == 'A' ||
            key == 's' || key == 'S' || key == 'd' || key == 'D') {
            direction = key;
        }
    }

    return direction;
}

int ConsoleInputReader::getSpellChoice(Hand* hand) {
    std::cout << "\nВыберите заклинание (0-" << (hand->size() - 1)
              << ") или -1 для отмены: ";

    int choice;
    std::cin >> choice;

    return choice;
}

void ConsoleInputReader::renderOptionsMenuFallback() {
    std::cout << "╔════════════════════════════════╗\n";
    std::cout << "║      МЕНЮ ОПЦИЙ                ║\n";
    std::cout << "╠════════════════════════════════╣\n";
    std::cout << "║ A - Атака                      ║\n";
    std::cout << "║ D - Переключить дальний бой    ║\n";
    std::cout << "║ S - Выбрать заклинание         ║\n";
    std::cout << "║ U - Прокачка персонажа         ║\n";
    std::cout << "║ ESC - Отмена                   ║\n";
    std::cout << "╚════════════════════════════════╝\n";
    std::cout << "\nВведите опцию: ";
}

void ConsoleInputReader::renderUpgradeMenuFallback(Player* player, Hand* hand) {
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║        🌟 ПРОКАЧКА ПЕРСОНАЖА 🌟        ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║  Доступно очков: " << (player ? player->GetUpgradePoints() : 0) << "                    ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║  1 - Увеличить макс. HP (+100) [1]     ║\n";
    std::cout << "║  2 - Увеличить урон (+2) [1]           ║\n";
    std::cout << "║  3 - Улучшить случайное заклинание     ║\n";
    std::cout << "║      (+50% урон/радиус) [2]            ║\n";
    std::cout << "║  ESC - Вернуться                       ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\n";
    if (player) {
        std::cout << "Текущие характеристики:\n";
        std::cout << "  HP: " << player->GetHealth() << " / " << player->GetMaxHealth() << "\n";
        std::cout << "  Урон: " << player->GetBaseDamage() << "\n";
    }
    std::cout << "  Заклинаний в руке: " << (hand ? hand->size() : 0) << "\n";
    std::cout << "\nВведите опцию: ";
}

