#include "Rendering/ImprovedGuiRenderer.h"

#ifdef _WIN32

#include "Board/Board.h"
#include "UI/GuiMenuSystem.h"
#include "Board/Cell.h"
#include "Board/Grid.h"
#include "Entity/EntityManager.h"
#include "Entity/Player/Player.h"
#include "Input/GuiInputReader.h"
#include "Magic/Hand.h"
#include "Magic/SpellCard.h"
#include "Magic/TrapRegistry.h"
#include "core/Level.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

namespace {
const wchar_t* kWindowClassName = L"ImprovedGameGuiWindowClass";
const wchar_t* kWindowTitle = L"Game - GUI Mode";
}

ImprovedGuiRenderer::ImprovedGuiRenderer() 
    : menuSystem(std::make_unique<GuiMenuSystem>()) {
    std::cout << "[GUI] ImprovedGuiRenderer constructor\n";
    startWindowThread();
}

void ImprovedGuiRenderer::setInputReader(GuiInputReader* reader) {
    std::cout << "[GUI] setInputReader called with " << (reader ? "valid" : "null") << " pointer\n";
    inputReader = reader;
}

void ImprovedGuiRenderer::forceRedraw() {
    if (hwnd) {
        InvalidateRect(hwnd, nullptr, TRUE);
        UpdateWindow(hwnd);
    }
}

ImprovedGuiRenderer::~ImprovedGuiRenderer() {
    stopWindowThread();
}

void ImprovedGuiRenderer::renderGameScreen(
    const Board* board,
    const Player* player,
    const Hand* hand,
    int moveCount
) {
    updateGameState(board, player, hand, moveCount);
}

void ImprovedGuiRenderer::renderOptionsMenu() {
    std::cout << "[GUI] renderOptionsMenu called\n";
    waitUntilWindowReady();
    
    if (menuSystem) {
        menuSystem->openOptionsMenu();
    }
    
    if (hwnd) {
        InvalidateRect(hwnd, nullptr, TRUE);
    }
}

void ImprovedGuiRenderer::renderUpgradeMenu(const Player* player, const Hand* /*hand*/) {
    std::lock_guard<std::mutex> lock(stateMutex);
    currentState.showUpgradeMenu = true;
    currentState.statusMessage = L"[Меню прокачки]";
    if (player) {
        currentState.upgradePoints = player->GetUpgradePoints();
    }
    if (hwnd) {
        PostMessage(hwnd, WM_UPDATE_STATE, 0, 0);
    }
}

void ImprovedGuiRenderer::renderMainMenu(bool hasSave) {
    std::cout << "[GUI] renderMainMenu called\n";
    waitUntilWindowReady();
    std::cout << "[GUI] Window ready\n";
    
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        currentState.statusMessage = L"=== ГЛАВНОЕ МЕНЮ ===\n\n";
        currentState.statusMessage += L"[1] Новая игра\n";
        currentState.statusMessage += hasSave ? L"[2] Загрузить игру\n" : L"[2] Загрузить игру (нет сохранения)\n";
        currentState.statusMessage += L"[Q] Выход\n\n";
        currentState.statusMessage += L"Используйте клавиши в окне GUI";
        
        // Очищаем поле для главного меню
        currentState.cells.clear();
        currentState.gridSize = 0;
    }
    
    std::cout << "[GUI] State updated, posting message\n";
    if (hwnd) {
        PostMessage(hwnd, WM_UPDATE_STATE, 0, 0);
    }
    std::cout << "[GUI] Message posted\n";
}

void ImprovedGuiRenderer::renderLevelStart(const Level* level) {
    waitUntilWindowReady();
    
    std::lock_guard<std::mutex> lock(stateMutex);
    currentState.cells.clear();
    currentState.gridSize = 0;
    
    if (level) {
        std::wostringstream oss;
        oss << L"=== НАЧАЛО УРОВНЯ ===\n\n";
        
        int len = MultiByteToWideChar(CP_UTF8, 0, level->getName().c_str(), -1, nullptr, 0);
        if (len > 0) {
            std::wstring wname(len, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, level->getName().c_str(), -1, wname.data(), len);
            if (!wname.empty() && wname.back() == L'\0') wname.pop_back();
            oss << L"Уровень: " << wname << L"\n\n";
        }
        
        len = MultiByteToWideChar(CP_UTF8, 0, level->getDescription().c_str(), -1, nullptr, 0);
        if (len > 0) {
            std::wstring wdesc(len, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, level->getDescription().c_str(), -1, wdesc.data(), len);
            if (!wdesc.empty() && wdesc.back() == L'\0') wdesc.pop_back();
            oss << wdesc << L"\n\n";
        }
        
        oss << L"Размер поля: " << level->getBoardSize() << L"x" << level->getBoardSize() << L"\n";
        oss << L"Сложность: " << level->getDifficulty() << L"/5\n\n";
        oss << L"Нажмите Enter в GUI окне для начала...";
        
        currentState.statusMessage = oss.str();
    }
    if (hwnd) {
        PostMessage(hwnd, WM_UPDATE_STATE, 0, 0);
    }
}

void ImprovedGuiRenderer::renderLevelComplete(const Level* /*level*/, int moves, int hp) {
    waitUntilWindowReady();
    
    std::lock_guard<std::mutex> lock(stateMutex);
    currentState.cells.clear();
    currentState.gridSize = 0;
    
    std::wostringstream oss;
    oss << L"=== УРОВЕНЬ ПРОЙДЕН! 🎉 ===\n\n";
    oss << L"Ходов: " << moves << L"\n";
    oss << L"HP: " << hp << L"\n\n";
    oss << L"Нажмите Enter для продолжения...";
    currentState.statusMessage = oss.str();
    if (hwnd) {
        PostMessage(hwnd, WM_UPDATE_STATE, 0, 0);
    }
}

void ImprovedGuiRenderer::renderGameComplete(int finalHp) {
    waitUntilWindowReady();
    
    std::lock_guard<std::mutex> lock(stateMutex);
    currentState.cells.clear();
    currentState.gridSize = 0;
    
    std::wostringstream oss;
    oss << L"=== ВЫ ПРОШЛИ ВСЮ ИГРУ! 🏆 ===\n\n";
    oss << L"Поздравляем!\n\n";
    oss << L"Финальный HP: " << finalHp << L"\n\n";
    oss << L"Спасибо за игру!";
    currentState.statusMessage = oss.str();
    if (hwnd) {
        PostMessage(hwnd, WM_UPDATE_STATE, 0, 0);
    }
}

void ImprovedGuiRenderer::renderGameOver(int moveCount) {
    waitUntilWindowReady();
    
    std::lock_guard<std::mutex> lock(stateMutex);
    currentState.cells.clear();
    currentState.gridSize = 0;
    
    std::wostringstream oss;
    oss << L"=== ПОРАЖЕНИЕ 💀 ===\n\n";
    oss << L"Вы продержались " << moveCount << L" ходов\n\n";
    oss << L"Попробуйте еще раз!";
    currentState.statusMessage = oss.str();
    if (hwnd) {
        PostMessage(hwnd, WM_UPDATE_STATE, 0, 0);
    }
}

void ImprovedGuiRenderer::renderVictory(int moveCount, int playerHealth) {
    std::lock_guard<std::mutex> lock(stateMutex);
    std::wostringstream oss;
    oss << L"Победа! Ходов: " << moveCount << L", HP: " << playerHealth;
    currentState.statusMessage = oss.str();
    if (hwnd) {
        PostMessage(hwnd, WM_UPDATE_STATE, 0, 0);
    }
}

void ImprovedGuiRenderer::clearScreen() {
    std::lock_guard<std::mutex> lock(stateMutex);
    currentState.cells.clear();
    currentState.statusMessage.clear();
    if (hwnd) {
        PostMessage(hwnd, WM_UPDATE_STATE, 0, 0);
    }
}

void ImprovedGuiRenderer::pause(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void ImprovedGuiRenderer::updateGameState(
    const Board* board,
    const Player* player,
    const Hand* hand,
    int moveCount
) {
    if (!board) return;

    GameState newState;
    newState.gridSize = board->getSize();
    newState.moveCount = moveCount;

    if (player) {
        newState.playerHealth = player->GetHealth();
        newState.playerMaxHealth = player->GetMaxHealth();
        newState.playerDamage = player->GetBaseDamage();
        newState.playerDistance = player->GetDistance();
        newState.upgradePoints = player->GetUpgradePoints();
    }

    const auto& entityManager = board->getEntityManager();
    newState.enemyCount = entityManager.getAliveEnemyCount();
    newState.buildingCount = entityManager.getActiveBuildingCount();

    // Собираем информацию о клетках
    Grid displayGrid = board->getGrid();
    int size = newState.gridSize;

    // Очищаем временную сетку
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            CellType current = displayGrid.getCell(x, y);
            if (current != CellType::Wall) {
                displayGrid.setCell(x, y, CellType::Empty);
            }
        }
    }

    // Башни и их радиусы
    const auto& towers = entityManager.getTowers();
    for (const auto& tower : towers) {
        if (!tower.isAlive() || !tower.canAttackNow()) continue;
        
        int tx = tower.getX();
        int ty = tower.getY();
        int radius = tower.getAttackRadius();

        for (int x = std::max(0, tx - radius); x <= std::min(size - 1, tx + radius); ++x) {
            for (int y = std::max(0, ty - radius); y <= std::min(size - 1, ty + radius); ++y) {
                int distX = std::abs(x - tx);
                int distY = std::abs(y - ty);
                if (std::max(distX, distY) <= radius && displayGrid.getCell(x, y) == CellType::Empty) {
                    displayGrid.setCell(x, y, CellType::TowerRange);
                }
            }
        }
    }

    // Ловушки
    for (const auto& trap : TrapRegistry::getAll()) {
        if (!trap.active) continue;
        if (trap.x >= 0 && trap.x < size && trap.y >= 0 && trap.y < size &&
            displayGrid.getCell(trap.x, trap.y) == CellType::Empty) {
            displayGrid.setCell(trap.x, trap.y, CellType::PlayerTrap);
        }
    }

    // Враги
    const auto& enemies = entityManager.getEnemies();
    const auto& enemyCoords = entityManager.getEnemyCoords();
    for (size_t i = 0; i < enemies.size() && i < enemyCoords.size(); ++i) {
        if (!enemies[i].IsAlive()) continue;
        int ex = enemyCoords[i].first;
        int ey = enemyCoords[i].second;
        if (ex >= 0 && ex < size && ey >= 0 && ey < size) {
            displayGrid.setCell(ex, ey, CellType::Enemy);
        }
    }

    // Здания
    const auto& buildings = entityManager.getBuildings();
    for (const auto& building : buildings) {
        if (building.getHealth() <= 0) continue;
        int bx = building.getX();
        int by = building.getY();
        if (bx >= 0 && bx < size && by >= 0 && by < size) {
            displayGrid.setCell(bx, by, CellType::HellBuild);
        }
    }

    // Башни (поверх радиуса)
    for (const auto& tower : towers) {
        if (tower.isAlive()) {
            int tx = tower.getX();
            int ty = tower.getY();
            if (tx >= 0 && tx < size && ty >= 0 && ty < size) {
                displayGrid.setCell(tx, ty, CellType::Tower);
            }
        }
    }

    // Союзники
    const auto& allies = entityManager.getAllies();
    const auto& allyCoords = entityManager.getAllyCoords();
    for (size_t i = 0; i < allies.size() && i < allyCoords.size(); ++i) {
        if (!allies[i].IsAlive()) continue;
        int ax = allyCoords[i].first;
        int ay = allyCoords[i].second;
        if (ax >= 0 && ax < size && ay >= 0 && ay < size) {
            displayGrid.setCell(ax, ay, CellType::Ally);
        }
    }

    // Игрок
    auto [px, py] = entityManager.getPlayerCoord();
    if (px >= 0 && px < size && py >= 0 && py < size) {
        displayGrid.setCell(px, py, CellType::Player);
    }

    // Преобразуем в CellInfo
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            CellType ct = displayGrid.getCell(x, y);
            CellInfo info;
            info.x = x;
            info.y = y;
            info.health = 0;

            switch (ct) {
                case CellType::Empty: info.type = 0; break;
                case CellType::Wall: info.type = 1; break;
                case CellType::Player: info.type = 2; break;
                case CellType::Enemy: info.type = 3; break;
                case CellType::HellBuild: info.type = 4; break;
                case CellType::Tower: info.type = 5; break;
                case CellType::PlayerTrap: info.type = 6; break;
                case CellType::Ally: info.type = 7; break;
                case CellType::TowerRange: info.type = 8; break;
                default: info.type = 0; break;
            }

            newState.cells.push_back(info);
        }
    }

    // Заклинания
    if (hand) {
        for (size_t i = 0; i < hand->size(); ++i) {
            if (SpellCard* spell = hand->getSpell(i)) {
                std::string name = spell->getName();
                int len = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, nullptr, 0);
                if (len > 0) {
                    std::wstring wname(len, L'\0');
                    MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wname.data(), len);
                    if (!wname.empty() && wname.back() == L'\0') {
                        wname.pop_back();
                    }
                    newState.spells.push_back(wname);
                }
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(stateMutex);
        currentState = std::move(newState);
    }

    if (hwnd) {
        PostMessage(hwnd, WM_UPDATE_STATE, 0, 0);
    }
}

void ImprovedGuiRenderer::startWindowThread() {
    running = true;
    windowThread = std::thread(&ImprovedGuiRenderer::windowThreadMain, this);
}

void ImprovedGuiRenderer::stopWindowThread() {
    running = false;
    if (hwnd) {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
    if (windowThread.joinable()) {
        windowThread.join();
    }
}

void ImprovedGuiRenderer::waitUntilWindowReady() {
    std::unique_lock<std::mutex> lock(readyMutex);
    readyCv.wait(lock, [this]() { return windowReady.load(); });
}

void ImprovedGuiRenderer::windowThreadMain() {
    instanceHandle = GetModuleHandle(nullptr);

    WNDCLASSW wc{};
    wc.lpfnWndProc = ImprovedGuiRenderer::WindowProc;
    wc.hInstance = instanceHandle;
    wc.lpszClassName = kWindowClassName;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassW(&wc);

    hwnd = CreateWindowExW(
        0,
        kWindowClassName,
        kWindowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1200, 800,
        nullptr,
        nullptr,
        instanceHandle,
        this
    );

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    {
        std::lock_guard<std::mutex> lock(readyMutex);
        windowReady = true;
    }
    readyCv.notify_all();
    
    // Начальная отрисовка
    InvalidateRect(hwnd, nullptr, TRUE);

    MSG msg{};
    while (running && GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    hwnd = nullptr;
}

LRESULT CALLBACK ImprovedGuiRenderer::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE) {
        auto* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
    }

    auto* renderer = reinterpret_cast<ImprovedGuiRenderer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_UPDATE_STATE:
            if (renderer) {
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;
        case WM_PAINT:
            if (renderer) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                renderer->onPaint(hdc);
                EndPaint(hwnd, &ps);
            }
            return 0;
        case WM_KEYDOWN:
            if (renderer) {
                renderer->onKeyDown(wParam);
            }
            return 0;
        case WM_LBUTTONDOWN:
            if (renderer) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                renderer->onMouseClick(x, y);
            }
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ImprovedGuiRenderer::onPaint(HDC hdc) {
    GameState state;
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        state = currentState;
    }

    // Отладка
    std::cout << "[GUI] onPaint called, gridSize=" << state.gridSize 
              << ", statusMsg=" << (state.statusMessage.empty() ? "empty" : "has text") << "\n";

    drawGrid(hdc, state);
    drawSidebar(hdc, state);
    
    // Рисуем меню поверх всего
    if (menuSystem && menuSystem->isMenuOpen()) {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        int menuWidth = 500;
        int menuHeight = 400;
        int menuX = (clientRect.right - menuWidth) / 2;
        int menuY = (clientRect.bottom - menuHeight) / 2;
        
        menuSystem->render(hdc, menuX, menuY, menuWidth, menuHeight);
    }
}

void ImprovedGuiRenderer::onKeyDown(WPARAM wParam) {
    std::cout << "[GUI] Key pressed: code=" << wParam << "\n";
    
    if (!inputReader) {
        std::cout << "[GUI] ERROR: inputReader is null!\n";
        return;
    }
    
    // Проверяем комбинации с Ctrl
    bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    
    std::string command;
    
    // Обработка комбинаций Ctrl+клавиша
    if (ctrlPressed) {
        if (wParam == 'S' || wParam == 's') {
            // Ctrl+S для сохранения
            command = "save";
            std::cout << "[GUI] Ctrl+S pressed - saving game\n";
            inputReader->pushCommand(command);
            return;
        }
    }
    
    // Преобразуем виртуальный код клавиши в символ
    if (wParam >= 'A' && wParam <= 'Z') {
        // Буквы - преобразуем в нижний регистр
        char ch = static_cast<char>(wParam - 'A' + 'a');
        command = std::string(1, ch);
        std::cout << "[GUI] Letter key: '" << ch << "'\n";
    } else if (wParam >= '0' && wParam <= '9') {
        // Цифры
        char ch = static_cast<char>(wParam);
        command = std::string(1, ch);
        std::cout << "[GUI] Digit key: '" << ch << "'\n";
    } else if (wParam == VK_ESCAPE) {
        // ESC - отправляем символ с кодом 27
        command = std::string(1, (char)27);
        std::cout << "[GUI] ESC key (code 27)\n";
    } else if (wParam == VK_RETURN) {
        // Enter
        command = std::string(1, '\n');
        std::cout << "[GUI] Enter key\n";
    } else if (wParam == VK_SPACE) {
        // Пробел
        command = std::string(1, ' ');
        std::cout << "[GUI] Space key\n";
    } else if (wParam == VK_F5) {
        // F5 для сохранения
        command = "save";
        std::cout << "[GUI] F5 pressed - saving game\n";
    } else {
        std::cout << "[GUI] Key not mapped (code=" << wParam << ")\n";
        return;
    }
    
    inputReader->pushCommand(command);
    std::cout << "[GUI] Pushed command to queue\n";
}

void ImprovedGuiRenderer::onMouseClick(int /*x*/, int /*y*/) {
    // Можно добавить клик по клеткам для выбора цели
}

void ImprovedGuiRenderer::drawGrid(HDC hdc, const GameState& state) {
    if (state.gridSize == 0) return;

    for (const auto& cell : state.cells) {
        int screenX = GRID_OFFSET_X + cell.x * CELL_SIZE;
        int screenY = GRID_OFFSET_Y + cell.y * CELL_SIZE;
        drawCell(hdc, screenX, screenY, cell.type, cell.health);
    }
}

void ImprovedGuiRenderer::drawSidebar(HDC hdc, const GameState& state) {
    int sidebarX = state.gridSize > 0 ? (GRID_OFFSET_X + state.gridSize * CELL_SIZE + 20) : 50;
    int y = 50;

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    
    // Если нет игрового поля (главное меню), показываем только статус
    if (state.gridSize == 0) {
        if (!state.statusMessage.empty()) {
            RECT rect;
            rect.left = sidebarX;
            rect.top = y;
            rect.right = 1000;
            rect.bottom = 700;
            
            HFONT hFont = CreateFontW(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
            
            DrawTextW(hdc, state.statusMessage.c_str(), -1, &rect,
                     DT_LEFT | DT_TOP | DT_WORDBREAK);
            
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
        }
        return;
    }

    // Заголовок
    std::wstring title = L"=== Статистика ===";
    TextOutW(hdc, sidebarX, y, title.c_str(), title.length());
    y += 30;

    // Ход
    std::wostringstream oss;
    oss << L"Ход: " << state.moveCount;
    std::wstring text = oss.str();
    TextOutW(hdc, sidebarX, y, text.c_str(), text.length());
    y += 25;

    // HP
    oss.str(L"");
    oss << L"HP: " << state.playerHealth << L"/" << state.playerMaxHealth;
    text = oss.str();
    TextOutW(hdc, sidebarX, y, text.c_str(), text.length());
    y += 25;

    // Урон
    oss.str(L"");
    oss << L"Урон: " << state.playerDamage;
    text = oss.str();
    TextOutW(hdc, sidebarX, y, text.c_str(), text.length());
    y += 25;

    // Дистанция
    oss.str(L"");
    oss << L"Дистанция: " << state.playerDistance;
    text = oss.str();
    TextOutW(hdc, sidebarX, y, text.c_str(), text.length());
    y += 25;

    // Очки прокачки
    oss.str(L"");
    oss << L"Очки прокачки: " << state.upgradePoints;
    text = oss.str();
    TextOutW(hdc, sidebarX, y, text.c_str(), text.length());
    y += 35;

    // Враги
    oss.str(L"");
    oss << L"Враги: " << state.enemyCount;
    text = oss.str();
    TextOutW(hdc, sidebarX, y, text.c_str(), text.length());
    y += 25;

    // Здания
    oss.str(L"");
    oss << L"Здания: " << state.buildingCount;
    text = oss.str();
    TextOutW(hdc, sidebarX, y, text.c_str(), text.length());
    y += 35;

    // Заклинания
    title = L"=== Заклинания ===";
    TextOutW(hdc, sidebarX, y, title.c_str(), title.length());
    y += 30;

    for (size_t i = 0; i < state.spells.size(); ++i) {
        oss.str(L"");
        oss << L"[" << i << L"] " << state.spells[i];
        text = oss.str();
        TextOutW(hdc, sidebarX, y, text.c_str(), text.length());
        y += 25;
    }

    y += 20;

    // Управление
    title = L"=== Управление ===";
    TextOutW(hdc, sidebarX, y, title.c_str(), title.length());
    y += 30;

    text = L"W/A/S/D - движение";
    TextOutW(hdc, sidebarX, y, text.c_str(), text.length());
    y += 25;

    text = L"O - меню опций";
    TextOutW(hdc, sidebarX, y, text.c_str(), text.length());
    y += 25;

    text = L"F5 или Ctrl+S - сохранить";
    TextOutW(hdc, sidebarX, y, text.c_str(), text.length());
    y += 25;

    text = L"Q - выход в меню";
    TextOutW(hdc, sidebarX, y, text.c_str(), text.length());
    y += 35;

    // Статус
    if (!state.statusMessage.empty()) {
        title = L"=== Статус ===";
        TextOutW(hdc, sidebarX, y, title.c_str(), title.length());
        y += 30;

        TextOutW(hdc, sidebarX, y, state.statusMessage.c_str(), state.statusMessage.length());
    }
}

void ImprovedGuiRenderer::drawCell(HDC hdc, int x, int y, int type, int health) {
    COLORREF color = getCellColor(type);
    
    HBRUSH brush = CreateSolidBrush(color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    Rectangle(hdc, x, y, x + CELL_SIZE, y + CELL_SIZE);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
    
    // Отображаем HP для врагов, зданий, башен и союзников
    if ((type == 3 || type == 4 || type == 5 || type == 7) && health > 0) {
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255)); // Белый текст
        
        HFONT hFont = CreateFontW(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        
        std::wstring healthText = std::to_wstring(health);
        RECT textRect = {x, y, x + CELL_SIZE, y + CELL_SIZE};
        DrawTextW(hdc, healthText.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
    }
}

COLORREF ImprovedGuiRenderer::getCellColor(int type) const {
    switch (type) {
        case 0: return RGB(255, 255, 255); // Empty - белый
        case 1: return RGB(80, 80, 80);    // Wall - серый
        case 2: return RGB(0, 200, 0);     // Player - зелёный
        case 3: return RGB(255, 0, 0);     // Enemy - красный
        case 4: return RGB(150, 0, 150);   // Building - фиолетовый
        case 5: return RGB(100, 50, 0);    // Tower - коричневый
        case 6: return RGB(255, 200, 0);   // Trap - жёлтый
        case 7: return RGB(0, 150, 255);   // Ally - голубой
        case 8: return RGB(255, 230, 230); // Tower range - светло-розовый
        default: return RGB(255, 255, 255);
    }
}

#endif // _WIN32

