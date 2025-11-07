#include "UI/MenuRenderer.h"
#include "Board/Board.h"
#include "Entity/Player/Player.h"
#include "Magic/Hand.h"
#include "Magic/SpellCard.h"
#include "UI/ConsoleUtils.h"
#include <iostream>
#include <iomanip>
#include <string>

const int MenuRenderer::WINDOW_WIDTH = 37;
const int MenuRenderer::WINDOW_HEIGHT = 25;

void MenuRenderer::renderGameScreen(const Board* board, const Player* player,
                                     const Hand* hand, int moveCount) {
    ConsoleUtils::clearScreen();

    if (player) {
        player->displayInfo();
    }

    if (board) {
        renderGameStats(board, player, moveCount);
        board->displayBoard();
    }

    renderSpellMenu(hand);
}

void MenuRenderer::renderStartScreen() {
    ConsoleUtils::clearScreen();

    std::cout << "╔═════════════════════════════════════╗\n";
    std::cout << "║                                     ║\n";
    std::cout << "║        🎮 DUNGEON CRAWLER 🎮      ║\n";
    std::cout << "║                                     ║\n";
    std::cout << "╚═════════════════════════════════════╝\n";
    std::cout << "\nДобро пожаловать в игру!\n\n";
}


void MenuRenderer::renderGameStats(const Board* board, const Player* player, int moveCount) {
    int aliveEnemies = board->getEntityManager().getAliveEnemyCount();
    int activeBuildings = board->getEntityManager().getActiveBuildingCount();

    std::cout << "\n📊 Цели: Враги: " << aliveEnemies
              << " | Здания: " << activeBuildings << "\n";
    std::cout << "═══════════════════════════════════════\n\n";
}

void MenuRenderer::renderSpellMenu(const Hand* hand) {
    if (!hand) return;


    if (hand->isEmpty()) {
        std::cout << "│  ✗ Нет доступных заклинаний    │\n";
    } else {
        for (size_t i = 0; i < hand->size(); ++i) {
            const SpellCard* spell = hand->getSpell(i);
            if (spell) {
                std::cout << "│ [" << i << "] ⚡ ";
                std::string name = spell->getName();

                // Выравнивание по длине
                if (name.length() < 23) {
                    name.append(23 - name.length(), ' ');
                }

                std::cout << name << "\n";
            }
        }

        // Пустые слоты
        for (size_t i = hand->size(); i < 5; ++i) {
            std::cout << "│ [" << i << "] ░ (пустой слот)        │\n";
        }
    }

}

void MenuRenderer::renderOptionsMenu() {
    std::cout << "╔════════════════════════════════╗\n";
    std::cout << "║      МЕНЮ ОПЦИЙ                ║\n";
    std::cout << "╠════════════════════════════════╣\n";
    std::cout << "║ A - Атака                      ║\n";
    std::cout << "║ D - Переключить дальний бой   ║\n";
    std::cout << "║ S - Выбрать заклинание        ║\n";
    std::cout << "║ ESC - Отмена                   ║\n";
    std::cout << "╚════════════════════════════════╝\n";
    std::cout << "\nВведите опцию: ";
}

void MenuRenderer::renderGameOver(int moveCount) {
    ConsoleUtils::clearScreen();
    std::cout << "═══════════════════════════════════════\n";
    std::cout << "        💀 GAME OVER! 💀              \n";
    std::cout << "═══════════════════════════════════════\n";
    std::cout << "Вы продержались " << moveCount << " ходов\n";
    std::cout << "═══════════════════════════════════════\n";
}

void MenuRenderer::renderVictory(int moveCount, int playerHealth) {
    ConsoleUtils::clearScreen();
    std::cout << "═══════════════════════════════════════\n";
    std::cout << "       🎉 ПОБЕДА! 🎉                 \n";
    std::cout << "═══════════════════════════════════════\n";
    std::cout << "Все враги повержены!\n";
    std::cout << "Все здания разрушены!\n";
    std::cout << "Потрачено ходов: " << moveCount << "\n";
    std::cout << "Осталось HP: " << playerHealth << "\n";
    std::cout << "═══════════════════════════════════════\n";
}

void MenuRenderer::renderSeparator(int width) {
    for (int i = 0; i < width; ++i) {
        std::cout << "═";
    }
    std::cout << "\n";
}

void MenuRenderer::renderBorder(const std::string& content, int width) {
    std::cout << "┌";
    for (int i = 0; i < width - 2; ++i) std::cout << "─";
    std::cout << "┐\n";

    std::cout << "│ " << std::setw(width - 4) << std::left << content << " │\n";

    std::cout << "└";
    for (int i = 0; i < width - 2; ++i) std::cout << "─";
    std::cout << "┘\n";
}
