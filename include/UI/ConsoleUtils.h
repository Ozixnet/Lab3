#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#include <string>

class ConsoleUtils {
public:
    // Очистить консоль
    static void clearScreen();
    
    // Пауза
    static void pause(int milliseconds);
    
    // Получить размер консоли
    static void getConsoleSize(int& width, int& height);
    
    // Установить цвет (Windows)
    static void setTextColor(int color);
    
    // Сброс цвета
    static void resetTextColor();
    
    // Установить позицию курсора (x, y) - координаты начинаются с (0, 0)
    static void setCursorPosition(int x, int y);

    // Константы для цветов
    static const int COLOR_BLACK;
    static const int COLOR_RED;
    static const int COLOR_GREEN;
    static const int COLOR_YELLOW;
    static const int COLOR_BLUE;
    static const int COLOR_MAGENTA;
    static const int COLOR_CYAN;
    static const int COLOR_WHITE;

private:
    ConsoleUtils() = delete;  // Утилитарный класс
};

#endif // CONSOLE_UTILS_H
