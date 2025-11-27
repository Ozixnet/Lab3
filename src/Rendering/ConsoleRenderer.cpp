#include "Rendering/ConsoleRenderer.h"

#include "Board/Board.h"
#include "Entity/EntityManager.h"
#include "Entity/Player/Player.h"
#include "Magic/Hand.h"
#include "Magic/SpellCard.h"
#include "UI/ConsoleUtils.h"
#include "core/Level.h"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <windows.h>

void ConsoleRenderer::renderGameScreen(
    const Board* board,
    const Player* player,
    const Hand* hand,
    int moveCount
) {
    clearScreen();

    if (player) {
        player->displayInfo();
    }

    if (board) {
        renderGameStats(board, player, moveCount);
    }

    renderSpellMenu(hand);

    std::cout << "\n";
    std::cout << "═══════════════════════════════════════\n";
    std::cout << "Команды: WASD-движение | O-меню | save-сохранить | load-загрузить | q-выход\n";
    std::cout << "═══════════════════════════════════════\n";

    if (board) {
        int consoleWidth = 0;
        int consoleHeight = 0;
        ConsoleUtils::getConsoleSize(consoleWidth, consoleHeight);

        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi {};
        int currentY = 0;
        if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
            currentY = csbi.dwCursorPosition.Y;
        }

        int boardSize = board->getSize();
        int estimatedBoardHeight = boardSize + 10;
        int targetY = consoleHeight - estimatedBoardHeight - 1;
        int emptyLines = std::max(0, targetY - currentY - 1);

        for (int i = 0; i < emptyLines; ++i) {
            std::cout << "\n";
        }

        board->displayBoard();
    }
}

void ConsoleRenderer::renderOptionsMenu() {
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

void ConsoleRenderer::renderUpgradeMenu(const Player* player, const Hand* hand) {
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

void ConsoleRenderer::renderMainMenu(bool hasSave) {
    clearScreen();
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════╗\n";
    std::cout << "║         ГЛАВНОЕ МЕНЮ                  ║\n";
    std::cout << "╠═══════════════════════════════════════╣\n";
    std::cout << "║  1. Новая игра                        ║\n";
    if (hasSave) {
        std::cout << "║  2. Загрузить игру                    ║\n";
    } else {
        std::cout << "║  2. Загрузить игру (нет сохранения)   ║\n";
    }
    std::cout << "║  0 или Q. Выход                       ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n";
    std::cout << "\nВыберите действие: ";
}

void ConsoleRenderer::renderLevelStart(const Level* level) {
    clearScreen();
    std::cout << "\n";
    if (!level) {
        std::cout << "Начало уровня\n";
        return;
    }

    std::cout << "╔═══════════════════════════════════════╗\n";
    std::cout << "║       " << level->getName() << "      ║\n";
    std::cout << "╠═══════════════════════════════════════╣\n";
    std::cout << "║  " << level->getDescription() << "    ║\n";
    std::cout << "╠═══════════════════════════════════════╣\n";
    std::cout << "║  Размер поля: " << level->getBoardSize() << "x" << level->getBoardSize()
              << "                 ║\n";
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
}

void ConsoleRenderer::renderLevelComplete(const Level* level, int moves, int hp) {
    clearScreen();
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════╗\n";
    std::cout << "║         УРОВЕНЬ ПРОЙДЕН! 🎉           ║\n";
    std::cout << "╠═══════════════════════════════════════╣\n";
    if (level) {
        std::cout << "║  " << level->getName() << "           ║\n";
    }
    std::cout << "║  Ходов: " << moves << "               ║\n";
    std::cout << "║  HP: " << hp << "                     ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n";
    std::cout << "\nНажмите Enter для продолжения...\n";
}

void ConsoleRenderer::renderGameComplete(int finalHp) {
    clearScreen();
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════╗\n";
    std::cout << "║       ВЫ ПРОШЛИ ВСЮ ИГРУ!🏆          ║\n";
    std::cout << "╠═══════════════════════════════════════╣\n";
    std::cout << "║  Все уровни пройдены!                 ║\n";
    std::cout << "║  Финальный HP: " << finalHp << "      ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n";
    std::cout << "\nСпасибо за игру!\n";
    std::cout << "Нажмите Enter...\n";
}

void ConsoleRenderer::renderGameOver(int moveCount) {
    clearScreen();
    std::cout << "═══════════════════════════════════════\n";
    std::cout << "        💀 GAME OVER! 💀              \n";
    std::cout << "═══════════════════════════════════════\n";
    std::cout << "Вы продержались " << moveCount << " ходов\n";
    std::cout << "═══════════════════════════════════════\n";
}

void ConsoleRenderer::renderVictory(int moveCount, int playerHealth) {
    clearScreen();
    std::cout << "═══════════════════════════════════════\n";
    std::cout << "       🎉 ПОБЕДА! 🎉                 \n";
    std::cout << "═══════════════════════════════════════\n";
    std::cout << "Все враги повержены!\n";
    std::cout << "Все здания разрушены!\n";
    std::cout << "Потрачено ходов: " << moveCount << "\n";
    std::cout << "Осталось HP: " << playerHealth << "\n";
    std::cout << "═══════════════════════════════════════\n";
}

void ConsoleRenderer::clearScreen() {
    ConsoleUtils::clearScreen();
}

void ConsoleRenderer::pause(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void ConsoleRenderer::renderGameStats(const Board* board, const Player* player, int moveCount) {
    if (!board) {
        return;
    }

    int aliveEnemies = board->getEntityManager().getAliveEnemyCount();
    int activeBuildings = board->getEntityManager().getActiveBuildingCount();

    std::cout << "\n📊 Цели: Враги: " << aliveEnemies
              << " | Здания: " << activeBuildings << "\n";
    std::cout << "Ход: " << moveCount;
    if (player) {
        std::cout << " | HP: " << player->GetHealth() << "/" << player->GetMaxHealth();
    }
    std::cout << "\n";
    renderSeparator(WINDOW_WIDTH);
    std::cout << "\n";
}

void ConsoleRenderer::renderSpellMenu(const Hand* hand) {
    if (!hand) {
        std::cout << "│  ✗ Нет данных о руке            │\n";
        return;
    }

    if (hand->isEmpty()) {
        std::cout << "│  ✗ Нет доступных заклинаний    │\n";
    } else {
        for (size_t i = 0; i < hand->size(); ++i) {
            const SpellCard* spell = hand->getSpell(i);
            if (!spell) {
                continue;
            }
            std::cout << "│ [" << i << "] ⚡ ";
            std::string name = spell->getName();
            if (name.length() < 23) {
                name.append(23 - name.length(), ' ');
            }
            std::cout << name << "\n";
        }

        for (size_t i = hand->size(); i < 5; ++i) {
            std::cout << "│ [" << i << "] ░ (пустой слот)        │\n";
        }
    }
}

void ConsoleRenderer::renderSeparator(int width) {
    for (int i = 0; i < width; ++i) {
        std::cout << "═";
    }
    std::cout << "\n";
}

void ConsoleRenderer::renderBorder(const std::string& content, int width) {
    std::cout << "┌";
    for (int i = 0; i < width - 2; ++i) {
        std::cout << "─";
    }
    std::cout << "┐\n";
    std::cout << "│ " << std::setw(width - 4) << std::left << content << " │\n";
    std::cout << "└";
    for (int i = 0; i < width - 2; ++i) {
        std::cout << "─";
    }
    std::cout << "┘\n";
}



