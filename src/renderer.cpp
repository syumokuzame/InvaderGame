#include "..\include\renderer.h"
#include "..\include\config.h"
#include <windows.h>
#include <cstring>

// バックバッファ（文字 + 属性）
static CHAR_INFO s_backBuf[Config::FIELD_HEIGHT * Config::FIELD_WIDTH];

Renderer::Renderer() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // スクリーンバッファサイズをフィールドに合わせる
    COORD bufSize = { Config::FIELD_WIDTH, Config::FIELD_HEIGHT };
    SetConsoleScreenBufferSize(hOut, bufSize);

    // ウィンドウサイズをフィールドに合わせる
    SMALL_RECT winRect = { 0, 0,
        static_cast<SHORT>(Config::FIELD_WIDTH - 1),
        static_cast<SHORT>(Config::FIELD_HEIGHT - 1) };
    SetConsoleWindowInfo(hOut, TRUE, &winRect);

    hideCursor();
    clear();
}

Renderer::~Renderer() {
    showCursor();
}

void Renderer::clear() {
    for (int i = 0; i < Config::FIELD_HEIGHT * Config::FIELD_WIDTH; ++i) {
        s_backBuf[i].Char.AsciiChar = ' ';
        s_backBuf[i].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    }
}

void Renderer::draw(int x, int y, char ch) {
    if (x < 0 || x >= Config::FIELD_WIDTH || y < 0 || y >= Config::FIELD_HEIGHT) return;
    s_backBuf[y * Config::FIELD_WIDTH + x].Char.AsciiChar = ch;
    s_backBuf[y * Config::FIELD_WIDTH + x].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
}

void Renderer::drawString(int x, int y, const char* str) {
    for (int i = 0; str[i] != '\0'; ++i)
        draw(x + i, y, str[i]);
}

void Renderer::present() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufSize   = { Config::FIELD_WIDTH, Config::FIELD_HEIGHT };
    COORD bufCoord  = { 0, 0 };
    SMALL_RECT rect = { 0, 0,
        static_cast<SHORT>(Config::FIELD_WIDTH - 1),
        static_cast<SHORT>(Config::FIELD_HEIGHT - 1) };
    WriteConsoleOutputA(hOut, s_backBuf, bufSize, bufCoord, &rect);
}

void Renderer::hideCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(hOut, &ci);
}

void Renderer::showCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci = { 1, TRUE };
    SetConsoleCursorInfo(hOut, &ci);
}
