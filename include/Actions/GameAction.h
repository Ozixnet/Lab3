#ifndef GAME_ACTION_H
#define GAME_ACTION_H


class Board;
class Player;

/**
 * @brief Базовый класс для всех игровых действий
 * 
 * Используется в системе ввода (требование 1).
 * IInputReader::parseCommand() создаёт объекты GameAction.
 * 
 * @see IInputReader
 * @see MoveAction
 * @see AttackAction
 * @see SpellAction
 */
class GameAction {
public:
    virtual ~GameAction() = default;

    // Выполнить действие
    // Возвращает результат:
    //   0 = успешное действие, завершает ход
    //   1 = атака попала
    //   2 = действие заблокировано, не завершает ход
    //   3 = ход пропущен из-за замедления
    //   4 = заклинание использовано
    //  -1 = действие не завершает ход (переключение)
    virtual int execute(Board& board, Player& player) = 0;
};

#endif // GAME_ACTION_H
