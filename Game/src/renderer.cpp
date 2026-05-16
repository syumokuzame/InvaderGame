#include "..\include\renderer.h"
#include "..\include\config.h"
#include <windows.h>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <algorithm>

namespace game {

// バックバッファ（文字 + 属性）
static CHAR_INFO s_backBuf[Config::FIELD_HEIGHT * Config::FIELD_WIDTH];

Renderer::Renderer() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to get standard output handle");
    }

    // 元のコンソール設定を保存
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) {
        throw std::runtime_error("Failed to get console screen buffer info");
    }
    originalBufferSize = csbi.dwSize;
    originalWindowRect = csbi.srWindow;
    originalAttributes = csbi.wAttributes;
    currentBufferSize = csbi.dwSize;  // 現在のバッファサイズを記録
    
    if (!GetConsoleCursorInfo(hOut, &originalCursorInfo)) {
        throw std::runtime_error("Failed to get console cursor info");
    }

    // ターミナル全体をクリア
    fullClear();

    // スクリーンバッファサイズをフィールドに合わせる（必要に応じて現在のサイズを使用）
    COORD bufSize = { Config::FIELD_WIDTH, Config::FIELD_HEIGHT };
    if (!SetConsoleScreenBufferSize(hOut, bufSize)) {
        // バッファサイズ設定が失敗した場合、現在のバッファサイズを使用
        // （コンソールのリダイレクトやターミナルの制約により失敗することがある）
        bufSize = csbi.dwSize;  // 現在のサイズを保持
    } else {
        currentBufferSize = bufSize;  // バッファサイズ変更成功時は記録
    }

    // ウィンドウサイズをフィールドに合わせる（バッファサイズ内に収まるよう調整）
    SMALL_RECT winRect = { 0, 0,
        static_cast<SHORT>((bufSize.X > Config::FIELD_WIDTH ? Config::FIELD_WIDTH : bufSize.X) - 1),
        static_cast<SHORT>((bufSize.Y > Config::FIELD_HEIGHT ? Config::FIELD_HEIGHT : bufSize.Y) - 1) };
    
    if (!SetConsoleWindowInfo(hOut, TRUE, &winRect)) {
        // ウィンドウサイズ設定も失敗した場合は、現在のウィンドウサイズでゲームを実行
        // ゲームロジックには影響しないので、エラーをスロー しない
    }

    hideCursor();
    clear();
}

Renderer::~Renderer() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        clearGameArea();
        showCursor();
        restoreConsoleSettings();
    }
}

void Renderer::fullClear() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    DWORD size = csbi.dwSize.X * csbi.dwSize.Y;
    COORD origin = { 0, 0 };
    DWORD written;
    FillConsoleOutputCharacterA(hOut, ' ', size, origin, &written);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, size, origin, &written);
    SetConsoleCursorPosition(hOut, origin);
}

void Renderer::clearGameArea() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD size = Config::FIELD_WIDTH * Config::FIELD_HEIGHT;
    COORD origin = { 0, 0 };
    DWORD written;
    FillConsoleOutputCharacterA(hOut, ' ', size, origin, &written);
    FillConsoleOutputAttribute(hOut, originalAttributes, size, origin, &written);
    SetConsoleCursorPosition(hOut, origin);
}

void Renderer::restoreConsoleSettings() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorInfo(hOut, &originalCursorInfo);
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
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    
    // 現在のコンソール情報を取得
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) {
        return;
    }
    
    // 実際のバッファサイズと描画領域を計算
    SHORT bufWidth = (csbi.dwSize.X < Config::FIELD_WIDTH) ? csbi.dwSize.X : Config::FIELD_WIDTH;
    SHORT bufHeight = (csbi.dwSize.Y < Config::FIELD_HEIGHT) ? csbi.dwSize.Y : Config::FIELD_HEIGHT;
    
    COORD bufSize = { bufWidth, bufHeight };
    COORD bufCoord = { 0, 0 };
    SMALL_RECT rect = { 0, 0, static_cast<SHORT>(bufWidth - 1), static_cast<SHORT>(bufHeight - 1) };
    
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

void Renderer::drawHeader(const char* title, int score, int elapsedSeconds) {
    // 1行目：タイトル（左端）、スコア（右側中央部）、時間（右端）
    drawString(1, 0, title);

    // スコア表示
    char scoreBuf[32];
    sprintf_s(scoreBuf, sizeof(scoreBuf), "Score: %d", score);
    int scoreX = Config::FIELD_WIDTH - 30;
    drawString(scoreX, 0, scoreBuf);

    // 時間表示（右端）
    char timeBuf[16];
    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;
    sprintf_s(timeBuf, sizeof(timeBuf), "%02d:%02d", minutes, seconds);
    int timeX = Config::FIELD_WIDTH - 8;
    drawString(timeX, 0, timeBuf);
}

void Renderer::drawInstructions() {
    // 2行目：操作方法表示
    const char* instructions = "LEFT/RIGHT: Move  SPACE: Shoot  P: Pause  Q: Quit";
    int instrX = 2;
    drawString(instrX, 1, instructions);
}

}  // namespace game
