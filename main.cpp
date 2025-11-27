#ifdef _WIN32

#include "Input/ConsoleInputReader.h"
#include "Logging/GameLogger.h"
#include "Rendering/ConsoleRenderer.h"
#include "core/GameController.h"

#include <string>

int main(int argc, char* argv[]) {
    LogMode logMode = LogMode::FILE;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--log-console") {
            logMode = LogMode::CONSOLE;
        } else if (arg == "--log-file") {
            logMode = LogMode::FILE;
        } else if (arg == "--log-both") {
            logMode = LogMode::BOTH;
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

    GameController<ConsoleInputReader, ConsoleRenderer> game;
    game.gameStart();
    return 0;
}
#endif
