#pragma once
#include <windows.h>

namespace engine {

// ダブルバッファリングを用いた描画管理
class Renderer {
public:
    Renderer();
    ~Renderer();

    void clear();
    void draw(int x, int y, char ch);
    void drawString(int x, int y, const char* str);
    void drawHeader(const char* title, int score, int elapsedSeconds);
    void drawInstructions();
    void present();

    void hideCursor();
    void showCursor();

private:
    COORD originalBufferSize;
    SMALL_RECT originalWindowRect;
    CONSOLE_CURSOR_INFO originalCursorInfo;
    WORD originalAttributes;
    COORD currentBufferSize;

    void fullClear();
    void clearGameArea();
    void restoreConsoleSettings();
};

}  // namespace engine
