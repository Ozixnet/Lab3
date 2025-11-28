#pragma once

#include "Input/IInputReader.h"
#include "Input/KeyBindings.h"
#include "Input/Direction.h"
#include <string>
#include <queue>
#include <mutex>

class GuiMenuSystem;

class GuiInputReader : public IInputReader {
public:
    GuiInputReader();

    void setRenderer(IGameRenderer* renderer) override;
    void setMenuSystem(GuiMenuSystem* menu) { menuSystem = menu; }
    GameCommand mapInputToCommand(const std::string& input) const override;
    std::string readRawInput() override;

    std::unique_ptr<GameAction> parseCommand(
        const std::string& input,
        Board* board,
        Hand* hand,
        Player* player
    ) override;

    // Методы для GUI
    void pushCommand(const std::string& cmd);
    bool hasCommand() const;

private:
    std::unique_ptr<GameAction> handleMovementInput(Direction direction);
    std::unique_ptr<GameAction> handleAttackInput(Board* board);
    std::unique_ptr<GameAction> handleSpellInput(Hand* hand, Board* board);
    std::unique_ptr<GameAction> handleOptionsMenu(Board* board, Hand* hand, Player* player);
    std::unique_ptr<GameAction> handleUpgradeMenu(Player* player, Hand* hand);

    KeyBindings keyBindings;
    IGameRenderer* renderer{nullptr};
    GuiMenuSystem* menuSystem{nullptr};
    
    std::queue<std::string> commandQueue;
    mutable std::mutex queueMutex;
};

