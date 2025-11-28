#include "Input/GuiInputReader.h"
#include "Input/Direction.h"
#include "Actions/MoveAction.h"
#include "Actions/AttackAction.h"
#include "Actions/SpellAction.h"
#include "Actions/ToggleRangeAction.h"
#include "Board/Board.h"
#include "Entity/Player/Player.h"
#include "Magic/Hand.h"
#include "Magic/SpellCard.h"
#include "Rendering/IGameRenderer.h"
#include "Rendering/ImprovedGuiRenderer.h"
#include "UI/GuiMenuSystem.h"
#include <iostream>
#include <thread>
#include <chrono>

GuiInputReader::GuiInputReader() {
    keyBindings.loadFromFile("config/keybindings.cfg");
}

void GuiInputReader::setRenderer(IGameRenderer* rendererPtr) {
    renderer = rendererPtr;
}

GameCommand GuiInputReader::mapInputToCommand(const std::string& input) const {
    return keyBindings.getCommand(input);
}

std::string GuiInputReader::readRawInput() {
    std::cout << "[GuiInputReader] Waiting for input...\n";
    // Ждём команду из GUI
    while (true) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!commandQueue.empty()) {
                std::string cmd = commandQueue.front();
                commandQueue.pop();
                std::cout << "[GuiInputReader] Got command: '" << cmd << "'\n";
                
                // Преобразуем "enter" в пустую строку для совместимости
                if (cmd == "enter") {
                    return "";
                }
                return cmd;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

std::unique_ptr<GameAction> GuiInputReader::parseCommand(
    const std::string& input,
    Board* board,
    Hand* hand,
    Player* player
) {
    if (input.empty()) {
        return nullptr;
    }

    GameCommand cmd = keyBindings.getCommand(input);

    switch (cmd) {
        // Передаём Direction (абстракция вместо символов)
        case GameCommand::MOVE_UP:
            return handleMovementInput(Direction::UP);
        case GameCommand::MOVE_DOWN:
            return handleMovementInput(Direction::DOWN);
        case GameCommand::MOVE_LEFT:
            return handleMovementInput(Direction::LEFT);
        case GameCommand::MOVE_RIGHT:
            return handleMovementInput(Direction::RIGHT);
        case GameCommand::OPEN_MENU:
            return handleOptionsMenu(board, hand, player);
        case GameCommand::QUIT:
        case GameCommand::SAVE:
        case GameCommand::LOAD:
            return nullptr;
        case GameCommand::NONE:
        default:
            break;
    }

    return nullptr;
}

void GuiInputReader::pushCommand(const std::string& cmd) {
    std::lock_guard<std::mutex> lock(queueMutex);
    commandQueue.push(cmd);
}

bool GuiInputReader::hasCommand() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return !commandQueue.empty();
}

std::unique_ptr<GameAction> GuiInputReader::handleMovementInput(Direction direction) {
    return std::make_unique<MoveAction>(direction);
}

std::unique_ptr<GameAction> GuiInputReader::handleAttackInput(Board* /*board*/) {
    // Для GUI версии можно упростить - атака в последнем направлении движения
    // Или добавить отдельные кнопки для направления атаки
    return nullptr;
}

std::unique_ptr<GameAction> GuiInputReader::handleSpellInput(Hand* /*hand*/, Board* /*board*/) {
    // Заглушка - в GUI будет отдельная панель выбора заклинаний
    std::cout << "[GuiInputReader] Spell menu not implemented yet\n";
    return nullptr;
}

std::unique_ptr<GameAction> GuiInputReader::handleUpgradeMenu(Player* /*player*/, Hand* /*hand*/) {
    // Заглушка - в GUI будет отдельная панель прокачки
    std::cout << "[GuiInputReader] Upgrade menu not implemented yet\n";
    return nullptr;
}

std::unique_ptr<GameAction> GuiInputReader::handleOptionsMenu(Board* board, Hand* hand, Player* player) {
    std::cout << "[GuiInputReader] handleOptionsMenu called\n";
    
    // Показываем меню через renderer
    if (renderer) {
        std::cout << "[GuiInputReader] Calling renderOptionsMenu\n";
        renderer->renderOptionsMenu();
    }
    
    if (!menuSystem) {
        std::cout << "[GuiInputReader] ERROR: menuSystem is null!\n";
        return nullptr;
    }
    
    // Ждём команды и обрабатываем через меню
    while (menuSystem->isMenuOpen()) {
        std::string input = readRawInput();
        if (input.empty()) continue;
        
        char key = input[0];
        std::cout << "[GuiInputReader] Processing key '" << key << "' (code=" << (int)key << ") in menu\n";
        
        std::unique_ptr<GameAction> action;
        
        bool actionReady = menuSystem->handleKeyPress(key, board, hand, player, action);
        std::cout << "[GuiInputReader] handleKeyPress returned " << actionReady 
                  << ", menu still open: " << menuSystem->isMenuOpen() << "\n";
        
        if (actionReady) {
            // Действие готово
            std::cout << "[GuiInputReader] Action ready, returning\n";
            // Принудительно перерисовываем окно, чтобы закрыть меню
            if (renderer) {
                ImprovedGuiRenderer* guiRenderer = dynamic_cast<ImprovedGuiRenderer*>(renderer);
                if (guiRenderer) {
                    guiRenderer->forceRedraw();
                }
            }
            return action;
        }
        
        // Меню обновилось или закрылось, перерисовываем
        if (renderer) {
            ImprovedGuiRenderer* guiRenderer = dynamic_cast<ImprovedGuiRenderer*>(renderer);
            if (guiRenderer) {
                std::cout << "[GuiInputReader] Forcing redraw after menu change\n";
                guiRenderer->forceRedraw();
            }
        }
    }
    
    std::cout << "[GuiInputReader] Menu closed, returning nullptr\n";
    return nullptr;
}

