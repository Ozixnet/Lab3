#include "Rendering/GuiRenderer.h"

#ifdef _WIN32

#include "Board/Board.h"
#include "Board/Grid.h"
#include "Board/Cell.h"
#include "Entity/EntityManager.h"
#include "Entity/Player/Player.h"
#include "Magic/Hand.h"
#include "Magic/SpellCard.h"
#include "Magic/TrapRegistry.h"
#include "core/Level.h"

#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {
const wchar_t* kWindowClassName = L"GameGuiRendererWindowClass";
const wchar_t* kWindowTitle = L"Game Visualization";
}

GuiRenderer::GuiRenderer() {
    startWindowThread();
    waitUntilWindowReady();
}

GuiRenderer::~GuiRenderer() {
    stopWindowThread();
}

void GuiRenderer::renderGameScreen(
    const Board* board,
    const Player* player,
    const Hand* hand,
    int moveCount
) {
    updateWindowText(buildGameSnapshot(board, player, hand, moveCount));
}

void GuiRenderer::renderOptionsMenu() {
    updateWindowText(currentText + L"\n\n[Открыто меню опций]");
}

void GuiRenderer::renderUpgradeMenu(const Player* player, const Hand* hand) {
    std::wostringstream oss;
    oss << L"=== Меню прокачки ===\n";
    if (player) {
        oss << L"Очки прокачки: " << player->GetUpgradePoints() << L"\n";
        oss << L"1. +100 HP\n2. +2 урона\n3. Улучшить случайное заклинание (2 очка)\n";
    } else {
        oss << L"Данные игрока недоступны\n";
    }
    if (hand) {
        oss << L"Заклинаний в руке: " << hand->size() << L"\n";
    }
    updateWindowText(oss.str());
}

void GuiRenderer::renderMainMenu(bool hasSave) {
    std::wostringstream oss;
    oss << L"=== Главное меню ===\n";
    oss << L"1. Новая игра\n";
    oss << L"2. Загрузить игру" << (hasSave ? L"" : L" (нет сохранения)") << L"\n";
    oss << L"0. Выход\n";
    updateWindowText(oss.str());
}

void GuiRenderer::renderLevelStart(const Level* level) {
    std::wostringstream oss;
    oss << L"=== Начало уровня ===\n";
    if (level) {
        oss << utf8ToWide(level->getName()) << L"\n";
        oss << utf8ToWide(level->getDescription()) << L"\n";
        oss << L"Размер поля: " << level->getBoardSize() << L"x" << level->getBoardSize() << L"\n";
        oss << L"Сложность: " << level->getDifficulty() << L"/5\n";
    }
    updateWindowText(oss.str());
}

void GuiRenderer::renderLevelComplete(const Level* level, int moves, int hp) {
    std::wostringstream oss;
    oss << L"=== Уровень пройден ===\n";
    if (level) {
        oss << utf8ToWide(level->getName()) << L"\n";
    }
    oss << L"Ходов: " << moves << L"\nHP: " << hp << L"\n";
    updateWindowText(oss.str());
}

void GuiRenderer::renderGameComplete(int finalHp) {
    std::wostringstream oss;
    oss << L"=== Игра завершена ===\n";
    oss << L"Поздравляем! Финальный HP: " << finalHp << L"\n";
    updateWindowText(oss.str());
}

void GuiRenderer::renderGameOver(int moveCount) {
    std::wostringstream oss;
    oss << L"=== Поражение ===\n";
    oss << L"Вы продержались " << moveCount << L" ходов\n";
    updateWindowText(oss.str());
}

void GuiRenderer::renderVictory(int moveCount, int playerHealth) {
    std::wostringstream oss;
    oss << L"=== Победа! ===\n";
    oss << L"Ходов: " << moveCount << L"\n";
    oss << L"HP игрока: " << playerHealth << L"\n";
    updateWindowText(oss.str());
}

void GuiRenderer::clearScreen() {
    updateWindowText(L"");
}

void GuiRenderer::pause(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void GuiRenderer::updateWindowText(const std::wstring& text) {
    waitUntilWindowReady();
    {
        std::lock_guard<std::mutex> lock(textMutex);
        pendingText = text;
    }
    if (hwnd) {
        PostMessage(hwnd, WM_UPDATE_TEXT, 0, 0);
    }
}

void GuiRenderer::startWindowThread() {
    if (running.load()) {
        return;
    }
    running = true;
    windowThread = std::thread(&GuiRenderer::windowThreadMain, this);
}

void GuiRenderer::stopWindowThread() {
    running = false;
    if (hwnd) {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
    if (windowThread.joinable()) {
        windowThread.join();
    }
    {
        std::lock_guard<std::mutex> lock(readyMutex);
        windowReady = false;
    }
}

void GuiRenderer::waitUntilWindowReady() {
    std::unique_lock<std::mutex> lock(readyMutex);
    readyCv.wait(lock, [this]() { return windowReady; });
}

void GuiRenderer::windowThreadMain() {
    instanceHandle = GetModuleHandle(nullptr);

    WNDCLASSW wc{};
    wc.lpfnWndProc = GuiRenderer::WindowProc;
    wc.hInstance = instanceHandle;
    wc.lpszClassName = kWindowClassName;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassW(&wc);

    hwnd = CreateWindowExW(
        0,
        kWindowClassName,
        kWindowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        900, 700,
        nullptr,
        nullptr,
        instanceHandle,
        this
    );

    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        {
            std::lock_guard<std::mutex> lock(readyMutex);
            windowReady = true;
        }
        readyCv.notify_all();
    } else {
        {
            std::lock_guard<std::mutex> lock(readyMutex);
            windowReady = true;
        }
        readyCv.notify_all();
        running = false;
        return;
    }

    MSG msg{};
    while (running && GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    hwnd = nullptr;
}

std::wstring GuiRenderer::buildGameSnapshot(
    const Board* board,
    const Player* player,
    const Hand* hand,
    int moveCount
) const {
    std::wostringstream oss;
    oss << L"=== Панель состояния ===\n";
    oss << L"Ход: " << moveCount << L"\n";

    if (player) {
        oss << L"Игрок: " << player->GetHealth() << L"/" << player->GetMaxHealth()
            << L" HP | Урон: " << player->GetBaseDamage();
        oss << L" | Дистанция: " << player->GetDistance() << L"\n";
    } else {
        oss << L"Нет данных об игроке\n";
    }

    if (board) {
        const auto& entityManager = board->getEntityManager();
        oss << L"Враги: " << entityManager.getAliveEnemyCount()
            << L" | Здания: " << entityManager.getActiveBuildingCount()
            << L" | Башни: " << entityManager.getTowerCount() << L"\n";
    } else {
        oss << L"Поле не инициализировано\n";
    }

    oss << L"\n=== Заклинания ===\n" << buildHandSection(hand) << L"\n";
    oss << L"=== Поле ===\n" << buildBoardPreview(board);
    return oss.str();
}

std::wstring GuiRenderer::buildBoardPreview(const Board* board) const {
    if (!board) {
        return L"Поле недоступно\n";
    }

    int size = board->getSize();
    Grid displayGrid = board->getGrid();

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            CellType current = displayGrid.getCell(x, y);
            if (current != CellType::Wall) {
                displayGrid.setCell(x, y, CellType::Empty);
            }
        }
    }

    const auto& entityManager = board->getEntityManager();
    const auto& towers = entityManager.getTowers();
    for (const auto& tower : towers) {
        if (!tower.isAlive() || !tower.canAttackNow()) {
            continue;
        }

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

    for (const auto& trap : TrapRegistry::getAll()) {
        if (!trap.active) {
            continue;
        }
        if (trap.x >= 0 && trap.x < size && trap.y >= 0 && trap.y < size &&
            displayGrid.getCell(trap.x, trap.y) == CellType::Empty) {
            displayGrid.setCell(trap.x, trap.y, CellType::PlayerTrap);
        }
    }

    const auto& enemies = entityManager.getEnemies();
    const auto& enemyCoords = entityManager.getEnemyCoords();
    for (size_t i = 0; i < enemies.size() && i < enemyCoords.size(); ++i) {
        if (!enemies[i].IsAlive()) {
            continue;
        }
        int ex = enemyCoords[i].first;
        int ey = enemyCoords[i].second;
        if (ex >= 0 && ex < size && ey >= 0 && ey < size) {
            displayGrid.setCell(ex, ey, CellType::Enemy);
        }
    }

    const auto& buildings = entityManager.getBuildings();
    for (const auto& building : buildings) {
        if (building.getHealth() <= 0) {
            continue;
        }
        int bx = building.getX();
        int by = building.getY();
        if (bx >= 0 && bx < size && by >= 0 && by < size) {
            displayGrid.setCell(bx, by, CellType::HellBuild);
        }
    }

    for (const auto& tower : towers) {
        if (tower.isAlive()) {
            int tx = tower.getX();
            int ty = tower.getY();
            if (tx >= 0 && tx < size && ty >= 0 && ty < size) {
                displayGrid.setCell(tx, ty, CellType::Tower);
            }
        }
    }

    const auto& allies = entityManager.getAllies();
    const auto& allyCoords = entityManager.getAllyCoords();
    for (size_t i = 0; i < allies.size() && i < allyCoords.size(); ++i) {
        if (!allies[i].IsAlive()) {
            continue;
        }
        int ax = allyCoords[i].first;
        int ay = allyCoords[i].second;
        if (ax >= 0 && ax < size && ay >= 0 && ay < size) {
            displayGrid.setCell(ax, ay, CellType::Ally);
        }
    }

    auto [px, py] = entityManager.getPlayerCoord();
    if (px >= 0 && px < size && py >= 0 && py < size) {
        displayGrid.setCell(px, py, CellType::Player);
    }

    std::wostringstream oss;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            oss << utf8ToWide(glyph(displayGrid.getCell(x, y))) << L" ";
        }
        oss << L"\n";
    }
    return oss.str();
}

std::wstring GuiRenderer::buildHandSection(const Hand* hand) const {
    if (!hand) {
        return L"Нет данных о руке\n";
    }
    if (hand->isEmpty()) {
        return L"Рука пуста\n";
    }

    std::wostringstream oss;
    for (size_t i = 0; i < hand->size(); ++i) {
        if (SpellCard* spell = hand->getSpell(i)) {
            oss << L"[" << i << L"] " << utf8ToWide(spell->getName()) << L"\n";
        } else {
            oss << L"[" << i << L"] (пусто)\n";
        }
    }
    return oss.str();
}

std::wstring GuiRenderer::utf8ToWide(const std::string& value) const {
    if (value.empty()) {
        return {};
    }
    int length = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
    if (length <= 0) {
        return {};
    }
    std::wstring result(static_cast<size_t>(length), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, result.data(), length);
    if (!result.empty() && result.back() == L'\0') {
        result.pop_back();
    }
    return result;
}

LRESULT CALLBACK GuiRenderer::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE) {
        auto* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
    }

    auto* renderer = reinterpret_cast<GuiRenderer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_UPDATE_TEXT: {
            std::wstring text;
            {
                std::lock_guard<std::mutex> lock(renderer->textMutex);
                text = renderer->pendingText;
            }
            renderer->currentText = std::move(text);
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rect;
            GetClientRect(hwnd, &rect);
            if (renderer) {
                DrawTextW(
                    hdc,
                    renderer->currentText.c_str(),
                    static_cast<int>(renderer->currentText.length()),
                    &rect,
                    DT_LEFT | DT_TOP | DT_NOPREFIX | DT_WORDBREAK
                );
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            if (renderer) {
                renderer->running = false;
            }
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#endif // _WIN32


