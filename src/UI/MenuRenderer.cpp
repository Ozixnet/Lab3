#include "UI/MenuRenderer.h"
#include "Board/Board.h"
#include "Entity/Player/Player.h"
#include "Magic/Hand.h"
#include "Magic/SpellCard.h"
#include "UI/ConsoleUtils.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <windows.h>

const int MenuRenderer::WINDOW_WIDTH = 37;
const int MenuRenderer::WINDOW_HEIGHT = 25;

void MenuRenderer::renderGameScreen(const Board* board, const Player* player,
                                     const Hand* hand, int moveCount) {
    ConsoleUtils::clearScreen();

    // 1. Статистика игрока
    if (player) {
        player->displayInfo();
    }

    // 2. Статистика уровня
    if (board) {
        renderGameStats(board, player, moveCount);
    }

    // 3. Меню заклинаний
    renderSpellMenu(hand);
    
    // 4. Подсказки по управлению
    std::cout << "\n";
    std::cout << "═══════════════════════════════════════\n";
    std::cout << "Команды: WASD-движение | O-меню | save-сохранить | load-загрузить | Q-выход\n";
    std::cout << "═══════════════════════════════════════\n";

    // 5. Заполнить пространство пустыми строками, чтобы поле было внизу экрана
    if (board) {
        // Получить размер консоли
        int consoleWidth, consoleHeight;
        ConsoleUtils::getConsoleSize(consoleWidth, consoleHeight);
        
        // Получить текущую позицию курсора
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int currentY = 0;
        if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
            currentY = csbi.dwCursorPosition.Y;
        }
        
        // Вычислить примерную высоту поля (размер поля + информация о врагах/зданиях)
        int boardSize = board->getSize();
        int estimatedBoardHeight = boardSize + 10; // поле + примерная высота информации
        
        // Вычислить сколько пустых строк нужно добавить, чтобы поле было внизу
        int targetY = consoleHeight - estimatedBoardHeight - 1;
        int emptyLines = std::max(0, targetY - currentY - 1);
        
        // Добавить пустые строки
        for (int i = 0; i < emptyLines; ++i) {
            std::cout << "\n";
        }
        
        // Вывести поле внизу
        board->displayBoard();
    }
}

void MenuRenderer::renderStartScreen() {
    ConsoleUtils::clearScreen();

    std::cout << "╔═════════════════════════════════════╗\n";
    std::cout << "║                                     ║\n";
    std::cout << "║        🎮 DUNGEON CRAWLER 🎮       ║\n";
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
    std::cout << "║ D - Переключить дальний бой    ║\n";
    std::cout << "║ S - Выбрать заклинание         ║\n";
    std::cout << "║ U - Прокачка персонажа         ║\n";
    std::cout << "║ ESC - Отмена                   ║\n";
    std::cout << "╚════════════════════════════════╝\n";
    std::cout << "\nВведите опцию: ";
}

void MenuRenderer::renderUpgradeMenu(const Player* player, const Hand* hand) {
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║        🌟 ПРОКАЧКА ПЕРСОНАЖА 🌟        ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║  Доступно очков: " << player->GetUpgradePoints() << "                    ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║  1 - Увеличить макс. HP (+100) [1]     ║\n";
    std::cout << "║  2 - Увеличить урон (+2) [1]           ║\n";
    std::cout << "║  3 - Улучшить случайное заклинание     ║\n";
    std::cout << "║      (+50% урон/радиус) [2]            ║\n";
    std::cout << "║  ESC - Вернуться                       ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Текущие характеристики:\n";
    std::cout << "  HP: " << player->GetHealth() << " / " << player->GetMaxHealth() << "\n";
    std::cout << "  Урон: " << player->GetBaseDamage() << "\n";
    std::cout << "  Заклинаний в руке: " << (hand ? hand->size() : 0) << "\n";
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
