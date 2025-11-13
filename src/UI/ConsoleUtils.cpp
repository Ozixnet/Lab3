#include "UI/ConsoleUtils.h"
#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>

// Константы цветов
const int ConsoleUtils::COLOR_BLACK = 0;
const int ConsoleUtils::COLOR_RED = FOREGROUND_RED;
const int ConsoleUtils::COLOR_GREEN = FOREGROUND_GREEN;
const int ConsoleUtils::COLOR_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN;
const int ConsoleUtils::COLOR_BLUE = FOREGROUND_BLUE;
const int ConsoleUtils::COLOR_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE;
const int ConsoleUtils::COLOR_CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE;
const int ConsoleUtils::COLOR_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

void ConsoleUtils::clearScreen() {
    system("cls");
}

void ConsoleUtils::pause(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void ConsoleUtils::getConsoleSize(int& width, int& height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        width = 80;
        height = 25;
    }
}

void ConsoleUtils::setTextColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color | FOREGROUND_INTENSITY);
}

void ConsoleUtils::resetTextColor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void ConsoleUtils::setCursorPosition(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = static_cast<SHORT>(x);
    coord.Y = static_cast<SHORT>(y);
    SetConsoleCursorPosition(hConsole, coord);
}
