#include "UI/InputHandler.h"
#include "Actions/MoveAction.h"
#include "Actions/AttackAction.h"
#include "Actions/SpellAction.h"
#include "Actions/ToggleRangeAction.h"
#include "Board/Board.h"
#include "Magic/Hand.h"
#include "Magic/SpellCard.h"
#include "UI/ConsoleUtils.h"
#include "UI/MenuRenderer.h"
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

std::unique_ptr<GameAction> InputHandler::handleOptionsMenu(Board* board, Hand* hand) {
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
        case 27:  // ESC
            std::cout << "Отменено\n";
            return nullptr;
        default:
            std::cout << "Неизвестная опция\n";
            ConsoleUtils::pause(1000);
            return nullptr;
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
