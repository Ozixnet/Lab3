#pragma once

#include "Rendering/IGameRenderer.h"

#ifdef _WIN32

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>

class GuiInputReader;
class GuiMenuSystem;

struct CellInfo {
    int x, y;
    int type; // 0=empty, 1=wall, 2=player, 3=enemy, 4=building, 5=tower, 6=trap, 7=ally, 8=tower_range
    int health;
};

class ImprovedGuiRenderer : public IGameRenderer {
public:
    ImprovedGuiRenderer();
    ~ImprovedGuiRenderer() override;

    void renderGameScreen(
        const Board* board,
        const Player* player,
        const Hand* hand,
        int moveCount
    ) override;

    void renderOptionsMenu() override;
    void renderUpgradeMenu(const Player* player, const Hand* hand) override;
    void renderMainMenu(bool hasSave) override;

    void renderLevelStart(const Level* level) override;
    void renderLevelComplete(const Level* level, int moves, int hp) override;
    void renderGameComplete(int finalHp) override;

    void renderGameOver(int moveCount) override;
    void renderVictory(int moveCount, int playerHealth) override;

    void clearScreen() override;
    void pause(int milliseconds) override;

    void setInputReader(GuiInputReader* reader);
    
    GuiInputReader* getInputReader() const { return inputReader; }
    GuiMenuSystem* getMenuSystem() { return menuSystem.get(); }
    
    void forceRedraw(); // Принудительная перерисовка окна

private:
    struct GameState {
        std::vector<CellInfo> cells;
        int gridSize{0};
        int playerHealth{0};
        int playerMaxHealth{0};
        int playerDamage{0};
        int playerDistance{1};
        int upgradePoints{0};
        int moveCount{0};
        int enemyCount{0};
        int buildingCount{0};
        std::vector<std::wstring> spells;
        std::wstring statusMessage;
        bool showMenu{false};
        bool showUpgradeMenu{false};
    };

    void updateGameState(
        const Board* board,
        const Player* player,
        const Hand* hand,
        int moveCount
    );
    
    void startWindowThread();
    void stopWindowThread();
    void waitUntilWindowReady();
    void windowThreadMain();

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void onPaint(HDC hdc);
    void onKeyDown(WPARAM wParam);
    void onMouseClick(int x, int y);
    
    void drawGrid(HDC hdc, const GameState& state);
    void drawSidebar(HDC hdc, const GameState& state);
    void drawCell(HDC hdc, int x, int y, int type, int health);
    COLORREF getCellColor(int type) const;

    HWND hwnd{nullptr};
    HINSTANCE instanceHandle{nullptr};
    
    GameState currentState;
    std::mutex stateMutex;
    std::thread windowThread;
    std::atomic<bool> running{false};
    std::atomic<bool> windowReady{false};
    std::mutex readyMutex;
    std::condition_variable readyCv;
    
    GuiInputReader* inputReader{nullptr};
    std::unique_ptr<GuiMenuSystem> menuSystem;

    static constexpr UINT WM_UPDATE_STATE = WM_USER + 1;
    static constexpr int CELL_SIZE = 28;
    static constexpr int SIDEBAR_WIDTH = 320;
    static constexpr int GRID_OFFSET_X = 10;
    static constexpr int GRID_OFFSET_Y = 10;
};

#else

class ImprovedGuiRenderer : public IGameRenderer {
public:
    void renderGameScreen(const Board*, const Player*, const Hand*, int) override {}
    void renderOptionsMenu() override {}
    void renderUpgradeMenu(const Player*, const Hand*) override {}
    void renderMainMenu(bool) override {}
    void renderLevelStart(const Level*) override {}
    void renderLevelComplete(const Level*, int, int) override {}
    void renderGameComplete(int) override {}
    void renderGameOver(int) override {}
    void renderVictory(int, int) override {}
    void clearScreen() override {}
    void pause(int) override {}
    void setInputReader(void*) {}
};

#endif // _WIN32

