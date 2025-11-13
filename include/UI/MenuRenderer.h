#ifndef MENU_RENDERER_H
#define MENU_RENDERER_H

#include <string>

class Hand;
class Board;
class Player;

class MenuRenderer {
public:
    // === ГЛАВНЫЕ ЭКРАНЫ ===

    // Основной экран игры
    static void renderGameScreen(const Board* board, const Player* player,
                                  const Hand* hand, int moveCount);

    // Экран начала игры
    static void renderStartScreen();

    // === КОМПОНЕНТЫ ===

    // Информация об игроке и статистика
    static void renderGameStats(const Board* board, const Player* player, int moveCount);

    // Меню заклинаний
    static void renderSpellMenu(const Hand* hand);

    // Меню опций
    static void renderOptionsMenu();
    
    // Меню прокачки
    static void renderUpgradeMenu(const Player* player, const Hand* hand);

    // === ИТОГОВЫЕ ЭКРАНЫ ===

    // Экран Game Over
    static void renderGameOver(int moveCount);

    // Экран победы
    static void renderVictory(int moveCount, int playerHealth);

    // === ВСПОМОГАТЕЛЬНЫЕ ===

    // Горизонтальная линия
    static void renderSeparator(int width = 37);

    // Рамка по сторонам
    static void renderBorder(const std::string& content, int width = 37);

private:
    MenuRenderer() = delete;  // Утилитарный класс

    static const int WINDOW_WIDTH;
    static const int WINDOW_HEIGHT;
};

#endif // MENU_RENDERER_H
