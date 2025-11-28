#include "UI/GuiMenuSystem.h"

#ifdef _WIN32

#include "Actions/AttackAction.h"
#include "Actions/MoveAction.h"
#include "Actions/SpellAction.h"
#include "Actions/ToggleRangeAction.h"
#include "Board/Board.h"
#include "Entity/Player/Player.h"
#include "Magic/Hand.h"
#include "Magic/SpellCard.h"
#include <iostream>

GuiMenuSystem::GuiMenuSystem() 
    : currentMenu(GuiMenuType::NONE)
    , selectedIndex(0) {
}

void GuiMenuSystem::openOptionsMenu() {
    std::cout << "[GuiMenu] Opening options menu\n";
    currentMenu = GuiMenuType::OPTIONS;
    buildOptionsMenu();
}

void GuiMenuSystem::openAttackMenu() {
    std::cout << "[GuiMenu] Opening attack menu\n";
    currentMenu = GuiMenuType::ATTACK_DIRECTION;
    buildAttackMenu();
}

void GuiMenuSystem::openSpellMenu(Hand* hand) {
    std::cout << "[GuiMenu] Opening spell menu\n";
    currentMenu = GuiMenuType::SPELL_SELECT;
    buildSpellMenu(hand);
}

void GuiMenuSystem::openUpgradeMenu(Player* player, Hand* hand) {
    std::cout << "[GuiMenu] Opening upgrade menu\n";
    currentMenu = GuiMenuType::UPGRADE;
    buildUpgradeMenu(player, hand);
}

void GuiMenuSystem::closeMenu() {
    std::cout << "[GuiMenu] Closing menu\n";
    currentMenu = GuiMenuType::NONE;
    menuItems.clear();
    selectedIndex = 0;
}

void GuiMenuSystem::buildOptionsMenu() {
    menuTitle = L"МЕНЮ ОПЦИЙ";
    menuItems.clear();
    
    menuItems.push_back({L"[A] Атака", 'a', 1});
    menuItems.push_back({L"[D] Переключить дальний бой", 'd', 2});
    menuItems.push_back({L"[S] Использовать заклинание", 's', 3});
    menuItems.push_back({L"[U] Прокачка персонажа", 'u', 4});
    menuItems.push_back({L"[ESC] Отмена", 27, 0});
}

void GuiMenuSystem::buildAttackMenu() {
    menuTitle = L"ВЫБОР НАПРАВЛЕНИЯ АТАКИ";
    menuItems.clear();
    
    menuItems.push_back({L"[W] Атака вверх ↑", 'w', 1});
    menuItems.push_back({L"[A] Атака влево ←", 'a', 2});
    menuItems.push_back({L"[S] Атака вниз ↓", 's', 3});
    menuItems.push_back({L"[D] Атака вправо →", 'd', 4});
    menuItems.push_back({L"[ESC] Отмена", 27, 0});
}

void GuiMenuSystem::buildSpellMenu(Hand* hand) {
    menuTitle = L"ВЫБОР ЗАКЛИНАНИЯ";
    menuItems.clear();
    
    if (!hand || hand->isEmpty()) {
        menuItems.push_back({L"Нет доступных заклинаний", 0, -1});
        menuItems.push_back({L"[ESC] Назад", 27, 0});
        return;
    }
    
    for (size_t i = 0; i < hand->size(); ++i) {
        if (SpellCard* spell = hand->getSpell(i)) {
            std::wstring text = L"[" + std::to_wstring(i) + L"] ";
            
            std::string name = spell->getName();
            int len = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, nullptr, 0);
            if (len > 0) {
                std::wstring wname(len, L'\0');
                MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wname.data(), len);
                if (!wname.empty() && wname.back() == L'\0') wname.pop_back();
                text += wname;
            }
            
            menuItems.push_back({text, static_cast<char>('0' + i), static_cast<int>(i) + 1});
        }
    }
    
    menuItems.push_back({L"[ESC] Отмена", 27, 0});
}

void GuiMenuSystem::buildUpgradeMenu(Player* player, Hand* /*hand*/) {
    menuTitle = L"ПРОКАЧКА ПЕРСОНАЖА";
    menuItems.clear();
    
    if (player) {
        std::wstring points = L"Доступно очков: " + std::to_wstring(player->GetUpgradePoints());
        menuItems.push_back({points, 0, -1});
        menuItems.push_back({L"", 0, -1}); // Пустая строка
    }
    
    menuItems.push_back({L"[1] Увеличить макс. HP (+100) [1 очко]", '1', 1});
    menuItems.push_back({L"[2] Увеличить урон (+2) [1 очко]", '2', 2});
    menuItems.push_back({L"[3] Улучшить случайное заклинание [2 очка]", '3', 3});
    menuItems.push_back({L"[ESC] Назад", 27, 0});
}

bool GuiMenuSystem::handleKeyPress(char key, Board* board, Hand* hand, Player* player, std::unique_ptr<GameAction>& outAction) {
    std::cout << "[GuiMenu] handleKeyPress: " << key << " in menu type " << (int)currentMenu << "\n";
    
    char lowerKey = key;
    if (key >= 'A' && key <= 'Z') {
        lowerKey = key - 'A' + 'a';
    }
    
    switch (currentMenu) {
        case GuiMenuType::OPTIONS:
            if (lowerKey == 'a') {
                openAttackMenu();
                return false;
            } else if (lowerKey == 'd') {
                outAction = std::make_unique<ToggleRangeAction>();
                closeMenu();
                return true;
            } else if (lowerKey == 's') {
                openSpellMenu(hand);
                return false;
            } else if (lowerKey == 'u') {
                openUpgradeMenu(player, hand);
                return false;
            } else if (key == 27) { // ESC
                closeMenu();
                return false;
            }
            break;
            
        case GuiMenuType::ATTACK_DIRECTION:
            if (lowerKey == 'w') {
                outAction = std::make_unique<AttackAction>('w');
                closeMenu();
                return true;
            } else if (lowerKey == 'a') {
                outAction = std::make_unique<AttackAction>('a');
                closeMenu();
                return true;
            } else if (lowerKey == 's') {
                outAction = std::make_unique<AttackAction>('s');
                closeMenu();
                return true;
            } else if (lowerKey == 'd') {
                outAction = std::make_unique<AttackAction>('d');
                closeMenu();
                return true;
            } else if (key == 27) {
                openOptionsMenu();
                return false;
            }
            break;
            
        case GuiMenuType::SPELL_SELECT:
            if (key >= '0' && key <= '9' && hand) {
                int index = key - '0';
                if (index < (int)hand->size()) {
                    SpellCard* spell = hand->getSpell(index);
                    if (spell && board) {
                        outAction = std::make_unique<SpellAction>(
                            spell,
                            &board->getEntityManager(),
                            hand,
                            board->getSize(),
                            index
                        );
                        closeMenu();
                        return true;
                    }
                }
            } else if (key == 27) {
                openOptionsMenu();
                return false;
            }
            break;
            
        case GuiMenuType::UPGRADE:
            if (player) {
                if (key == '1' && player->SpendUpgradePoint()) {
                    player->UpgradeMaxHealth(100);
                    std::cout << "✅ Максимальное HP увеличено на 100!\n";
                    buildUpgradeMenu(player, hand); // Обновляем меню
                    return false;
                } else if (key == '2' && player->SpendUpgradePoint()) {
                    player->UpgradeBaseDamage(2);
                    std::cout << "✅ Базовый урон увеличен на 2!\n";
                    buildUpgradeMenu(player, hand);
                    return false;
                } else if (key == '3' && player->GetUpgradePoints() >= 2) {

                    std::cout << "⚠️ Улучшение заклинаний пока не реализовано\n";
                    return false;
                }
            }
            if (key == 27) {
                openOptionsMenu();
                return false;
            }
            break;
            
        default:
            break;
    }
    
    return false;
}

void GuiMenuSystem::render(HDC hdc, int x, int y, int width, int height) {
    if (currentMenu == GuiMenuType::NONE) return;
    
    drawMenuBox(hdc, x, y, width, height);
    
    // Заголовок
    HFONT hTitleFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hTitleFont);
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    
    RECT titleRect = {x + 20, y + 20, x + width - 20, y + 60};
    DrawTextW(hdc, menuTitle.c_str(), -1, &titleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hTitleFont);
    
    // Элементы меню
    HFONT hItemFont = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                  DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
    SelectObject(hdc, hItemFont);
    
    int itemY = y + 80;
    for (size_t i = 0; i < menuItems.size(); ++i) {
        drawMenuItem(hdc, x + 30, itemY, menuItems[i], i == (size_t)selectedIndex);
        itemY += 35;
    }
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hItemFont);
}

void GuiMenuSystem::drawMenuBox(HDC hdc, int x, int y, int width, int height) {
    // Полупрозрачный фон (эмуляция через серый)
    HBRUSH bgBrush = CreateSolidBrush(RGB(40, 40, 60));
    HPEN borderPen = CreatePen(PS_SOLID, 3, RGB(100, 150, 255));
    
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, bgBrush);
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
    
    Rectangle(hdc, x, y, x + width, y + height);
    
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(bgBrush);
    DeleteObject(borderPen);
}

void GuiMenuSystem::drawMenuItem(HDC hdc, int x, int y, const GuiMenuItem& item, bool selected) {
    if (item.hotkey == 0 && item.id == -1) {
        // Информационная строка
        SetTextColor(hdc, RGB(200, 200, 200));
    } else if (selected) {
        SetTextColor(hdc, RGB(255, 255, 0));
    } else {
        SetTextColor(hdc, RGB(255, 255, 255));
    }
    
    TextOutW(hdc, x, y, item.text.c_str(), item.text.length());
}

#endif // _WIN32

