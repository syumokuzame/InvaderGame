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
    COORD mOriginalBufferSize;
    SMALL_RECT mOriginalWindowRect;
    CONSOLE_CURSOR_INFO mOriginalCursorInfo;
    WORD mOriginalAttributes;
    COORD mCurrentBufferSize;

    void fullClear();
    void clearGameArea();
    void restoreConsoleSettings();
};

}  // namespace engine
