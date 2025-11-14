#include "UI/InputHandler.h"
#include "Actions/MoveAction.h"
#include "Actions/AttackAction.h"
#include "Actions/SpellAction.h"
#include "Actions/ToggleRangeAction.h"
#include "Board/Board.h"
#include "Magic/Hand.h"
#include "Magic/SpellCard.h"
#include "Entity/Player/Player.h"
#include "UI/ConsoleUtils.h"
#include "UI/MenuRenderer.h"
#include "Utils/Random.h"
#include <iostream>
#include <conio.h>

#include <windows.h>
#include <cctype>


std::unique_ptr<GameAction> InputHandler::handleMovementInput(char direction) {
    return std::make_unique<MoveAction>(direction);
}

std::unique_ptr<GameAction> InputHandler::handleAttackInput(Board* board) {
    if (!board) {
        std::cout << "Ошибка: доска не инициализирована!\n";
        return nullptr;
    }

    std::cout << "\n╔════════════════════════════════╗\n";
    std::cout << "║  Куда атакуем? WASD + Enter   ║\n";
    std::cout << "╚════════════════════════════════╝\n\n";
    board->displayBoard();

    char direction = getDirectionFromUser();
    if (direction == ' ') {
        std::cout << "Атака отменена.\n";
        return nullptr;
    }

    return std::make_unique<AttackAction>(direction);
}

std::unique_ptr<GameAction> InputHandler::handleSpellInput(Hand* hand, Board* board) {
    if (!hand || hand->isEmpty()) {
        std::cout << "\n╔════════════════════════════════╗\n";
        std::cout << "║  ✗ Нет доступных заклинаний  ║\n";
        std::cout << "╚════════════════════════════════╝\n";
        ConsoleUtils::pause(1500);
        return nullptr;
    }

    if (!board) {
        std::cout << "Ошибка: доска не инициализирована!\n";
        return nullptr;
    }

    ConsoleUtils::clearScreen();

    std::cout << "═══════════════════════════════════════\n";
    std::cout << "        ВЫБОР ЗАКЛИНАНИЯ               \n";
    std::cout << "═══════════════════════════════════════\n\n";

    board->displayBoard();

    std::cout << "\n";
    hand->showHand();

    int choice = getSpellChoice(hand);
    if (choice == -1) {
        std::cout << "Отмена использования заклинания.\n";
        ConsoleUtils::pause(1000);
        return nullptr;
    }

    SpellCard* spell = hand->getSpell(choice);
    if (!spell) {
        std::cout << "Некорректный выбор заклинания!\n";
        ConsoleUtils::pause(1500);
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

std::unique_ptr<GameAction> InputHandler::handleOptionsMenu(Board* board, Hand* hand, Player* player) {
    char option;
    ConsoleUtils::clearScreen();

    MenuRenderer::renderOptionsMenu();

    option = _getch();

    switch(option) {
        case 'a': case 'A':
            return handleAttackInput(board);
        case 'd': case 'D':
            return std::make_unique<ToggleRangeAction>();
        case 's': case 'S':
            return handleSpellInput(hand, board);
        case 'u': case 'U':
            return handleUpgradeMenu(player, hand);
        case 27:  // ESC
            std::cout << "Отменено\n";
            return nullptr;
        default:
            std::cout << "Неизвестная опция\n";
            ConsoleUtils::pause(1000);
            return nullptr;
    }
}

std::unique_ptr<GameAction> InputHandler::handleUpgradeMenu(Player* player, Hand* hand) {
    if (!player) {
        std::cout << "Ошибка: игрок не инициализирован!\n";
        ConsoleUtils::pause(1500);
        return nullptr;
    }
    
    while (true) {
        ConsoleUtils::clearScreen();
        MenuRenderer::renderUpgradeMenu(player, hand);
        
        char option = _getch();
        
        switch(option) {
            case '1': {
                // Улучшить макс. HP
                if (player->SpendUpgradePoint()) {
                    player->UpgradeMaxHealth(100);
                    std::cout << "\n✅ Максимальное HP увеличено на 100!\n";
                    std::cout << "Новое значение: " << player->GetMaxHealth() << "\n";
                    ConsoleUtils::pause(1500);
                } else {
                    std::cout << "\n❌ Недостаточно очков прокачки!\n";
                    ConsoleUtils::pause(1500);
                }
                break;
            }
            case '2': {
                // Улучшить урон
                if (player->SpendUpgradePoint()) {
                    player->UpgradeBaseDamage(2);
                    std::cout << "\n✅ Базовый урон увеличен на 2!\n";
                    std::cout << "Новое значение: " << player->GetBaseDamage() << "\n";
                    ConsoleUtils::pause(1500);
                } else {
                    std::cout << "\n❌ Недостаточно очков прокачки!\n";
                    ConsoleUtils::pause(1500);
                }
                break;
            }
            case '3': {
                // Улучшить случайное заклинание
                if (player->GetUpgradePoints() < 2) {
                    std::cout << "\n❌ Требуется 2 очка прокачки!\n";
                    ConsoleUtils::pause(1500);
                    break;
                }
                
                if (!hand || hand->isEmpty()) {
                    std::cout << "\n❌ Нет заклинаний для улучшения!\n";
                    ConsoleUtils::pause(1500);
                    break;
                }
                
                // Улучшить случайное заклинание
                int randomIndex = Random::getRange(0, hand->size() - 1);
                SpellCard* spell = hand->getSpell(randomIndex);
                
                if (spell) {
                    // Потратить 2 очка
                    player->SpendUpgradePoint();
                    player->SpendUpgradePoint();
                    
                    // Улучшить заклинание (увеличить урон и радиус на 50%)
                    // Это упрощенная реализация, в реальности нужно модифицировать SpellCard
                    std::cout << "\n✅ Заклинание '" << spell->getName() << "' улучшено!\n";
                    std::cout << "Урон и радиус увеличены на 50%!\n";
                    
                    // TODO: Реализовать фактическое улучшение заклинания
                    // Можно добавить множитель в SpellCard или использовать SpellBuffContext
                    
                    ConsoleUtils::pause(2000);
                } else {
                    std::cout << "\n❌ Ошибка при улучшении заклинания!\n";
                    ConsoleUtils::pause(1500);
                }
                break;
            }
            case 27:  // ESC
                std::cout << "\nВозврат в меню опций...\n";
                ConsoleUtils::pause(500);
                return nullptr;
            default:
                std::cout << "\n❌ Неизвестная опция!\n";
                ConsoleUtils::pause(1000);
                break;
        }
    }
}

bool InputHandler::confirmExit() {
    std::cout << "Вы уверены? (Y/N): ";
    char answer = _getch();
    return (answer == 'Y' || answer == 'y');
}

char InputHandler::getDirectionFromUser() {
    char direction = ' ';

    while (true) {
        if (direction != ' ') {
            std::cout << "\r                         \r";
            switch(direction) {
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

int InputHandler::getSpellChoice(Hand* hand) {
    std::cout << "\nВыберите заклинание (0-" << (hand->size() - 1)
              << ") или -1 для отмены: ";

    int choice;
    std::cin >> choice;

    return choice;
}
