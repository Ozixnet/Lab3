#ifndef DIRECTION_H
#define DIRECTION_H

/**
 * @brief Абстракция направления движения
 * 
 * Заменяет работу с символами ('w', 's', 'a', 'd') на типобезопасный enum.
 * Используется в MoveAction и EntityManager::playerMove.
 */
enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

/**
 * @brief Преобразование Direction в смещение по координатам
 * @param dir Направление
 * @param dx Выходной параметр - смещение по X
 * @param dy Выходной параметр - смещение по Y
 */
inline void directionToOffset(Direction dir, int& dx, int& dy) {
    dx = 0;
    dy = 0;
    switch (dir) {
        case Direction::UP:    dy = -1; break;
        case Direction::DOWN:  dy = +1; break;
        case Direction::LEFT:  dx = -1; break;
        case Direction::RIGHT: dx = +1; break;
        case Direction::NONE:  break;
    }
}

/**
 * @brief Преобразование символа в Direction (для обратной совместимости)
 * @param c Символ направления ('w', 'a', 's', 'd')
 * @return Соответствующее Direction или NONE
 */
inline Direction charToDirection(char c) {
    switch (c) {
        case 'w': case 'W': return Direction::UP;
        case 's': case 'S': return Direction::DOWN;
        case 'a': case 'A': return Direction::LEFT;
        case 'd': case 'D': return Direction::RIGHT;
        default: return Direction::NONE;
    }
}

/**
 * @brief Преобразование Direction в строку (для отладки/логирования)
 */
inline const char* directionToString(Direction dir) {
    switch (dir) {
        case Direction::UP:    return "UP";
        case Direction::DOWN:  return "DOWN";
        case Direction::LEFT:  return "LEFT";
        case Direction::RIGHT: return "RIGHT";
        case Direction::NONE:  return "NONE";
    }
    return "UNKNOWN";
}

#endif // DIRECTION_H


