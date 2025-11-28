#pragma once

#include "Rendering/IGameRenderer.h"

#ifdef _WIN32

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <windows.h>

class GuiRenderer : public IGameRenderer {
public:
    GuiRenderer();
    ~GuiRenderer() override;

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

private:
    void startWindowThread();
    void stopWindowThread();
    void waitUntilWindowReady();
    void windowThreadMain();
    void updateWindowText(const std::wstring& text);
    std::wstring buildGameSnapshot(
        const Board* board,
        const Player* player,
        const Hand* hand,
        int moveCount
    ) const;
    std::wstring buildBoardPreview(const Board* board) const;
    std::wstring buildHandSection(const Hand* hand) const;
    std::wstring utf8ToWide(const std::string& value) const;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static constexpr UINT WM_UPDATE_TEXT = WM_APP + 1;

    HWND hwnd{nullptr};
    HINSTANCE instanceHandle{nullptr};
    std::wstring currentText;

    std::atomic<bool> running{false};
    std::thread windowThread;
    std::mutex textMutex;
    std::wstring pendingText;

    std::mutex readyMutex;
    std::condition_variable readyCv;
    bool windowReady{false};
};

#else

// Заглушка для других платформ (сохраняем сборку, даже если GUI не поддерживается)
class GuiRenderer : public IGameRenderer {
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
};

#endif // _WIN32


