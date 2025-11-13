#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <memory>

class GameAction;
class Board;
class Hand;
class Player;

class InputHandler {
public:
    // === ОСНОВНЫЕ ДЕЙСТВИЯ ===

    // Обработать движение (WASD)
    static std::unique_ptr<GameAction> handleMovementInput(char direction);

    // Обработать атаку (требует выбора направления)
    static std::unique_ptr<GameAction> handleAttackInput(Board* board);

    // Обработать выбор заклинания
    static std::unique_ptr<GameAction> handleSpellInput(Hand* hand, Board* board);

    // === МЕНЮ ===

    // Главное меню опций
    static std::unique_ptr<GameAction> handleOptionsMenu(Board* board, Hand* hand, Player* player);

    // Меню улучшений
    static std::unique_ptr<GameAction> handleUpgradeMenu(Player* player, Hand* hand);

    // Подтверждение выхода
    static bool confirmExit();

private:
    InputHandler() = delete;  // Утилитарный класс

    // Приватные вспомогательные методы
    static char getDirectionFromUser();
    static int getSpellChoice(Hand* hand);
};

#endif // INPUT_HANDLER_H
