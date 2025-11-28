#ifdef _WIN32

#include "Input/ConsoleInputReader.h"
#include "Input/GuiInputReader.h"
#include "Logging/GameLogger.h"
#include "Rendering/ConsoleRenderer.h"
#include "Rendering/ImprovedGuiRenderer.h"
#include "core/GameController.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    LogMode logMode = LogMode::FILE;
    bool useGuiRenderer = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--log-console") {
            logMode = LogMode::CONSOLE;
        } else if (arg == "--log-file") {
            logMode = LogMode::FILE;
        } else if (arg == "--log-both") {
            logMode = LogMode::BOTH;
        } else if (arg == "--gui" || arg == "--renderer=gui") {
            useGuiRenderer = true;
        }
    }

    GameLogger logger(logMode, "logs/game.log");

    std::cout << "Режим логирования: ";
    switch (logMode) {
        case LogMode::CONSOLE:
            std::cout << "консоль\n";
            break;
        case LogMode::FILE:
            std::cout << "файл (logs/game.log)\n";
            break;
        case LogMode::BOTH:
            std::cout << "консоль + файл\n";
            break;
    }

    if (useGuiRenderer) {
        std::cout << "Запуск в GUI режиме (графический интерфейс)...\n";
        GameController<GuiInputReader, ImprovedGuiRenderer> game;
        // Связываем GUI input reader с GUI renderer и menu system
        game.getGameView().getRenderer().setInputReader(&game.getInputReader());
        game.getInputReader().setMenuSystem(game.getGameView().getRenderer().getMenuSystem());
        game.gameStart();
    } else {
        std::cout << "Запуск в консольном режиме...\n";
    GameController<ConsoleInputReader, ConsoleRenderer> game;
    game.gameStart();
    }
    return 0;
}
#endif
