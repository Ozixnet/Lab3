#pragma once

#ifdef _WIN32

#include <windows.h>
#include <string>
#include <vector>
#include <memory>

class Board;
class Hand;
class Player;
class GameAction;

enum class GuiMenuType {
    NONE,
    OPTIONS,
    ATTACK_DIRECTION,
    SPELL_SELECT,
    UPGRADE
};

struct GuiMenuItem {
    std::wstring text;
    char hotkey;
    int id;
};

class GuiMenuSystem {
public:
    GuiMenuSystem();
    
    void openOptionsMenu();
    void openAttackMenu();
    void openSpellMenu(Hand* hand);
    void openUpgradeMenu(Player* player, Hand* hand);
    void closeMenu();
    
    bool isMenuOpen() const { return currentMenu != GuiMenuType::NONE; }
    GuiMenuType getCurrentMenu() const { return currentMenu; }
    
    void render(HDC hdc, int x, int y, int width, int height);
    
    // Обработка ввода
    bool handleKeyPress(char key, Board* board, Hand* hand, Player* player, std::unique_ptr<GameAction>& outAction);
    
    const std::vector<GuiMenuItem>& getMenuItems() const { return menuItems; }
    std::wstring getMenuTitle() const { return menuTitle; }
    
private:
    GuiMenuType currentMenu;
    std::vector<GuiMenuItem> menuItems;
    std::wstring menuTitle;
    int selectedIndex;
    
    void buildOptionsMenu();
    void buildAttackMenu();
    void buildSpellMenu(Hand* hand);
    void buildUpgradeMenu(Player* player, Hand* hand);
    
    void drawMenuBox(HDC hdc, int x, int y, int width, int height);
    void drawMenuItem(HDC hdc, int x, int y, const GuiMenuItem& item, bool selected);
};

#endif // _WIN32

