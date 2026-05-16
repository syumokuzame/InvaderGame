#pragma once
#include <windows.h>

// ダブルバッファリングを用いた描画管理
class Renderer {
public:
    Renderer();
    ~Renderer();

    void clear();           // バックバッファをクリア
    void draw(int x, int y, char ch);   // 指定座標に文字を描画
    void drawString(int x, int y, const char* str);
    void drawHeader(const char* title, int score, int elapsedSeconds);  // 上部UI描画（1-2行目）
    void drawInstructions();  // 操作方法表示（2行目）
    void present();         // バックバッファをフロントに転送（描画確定）

    void hideCursor();
    void showCursor();

private:
    // コンソール設定を保存
    COORD originalBufferSize;
    SMALL_RECT originalWindowRect;
    CONSOLE_CURSOR_INFO originalCursorInfo;
    WORD originalAttributes;
    COORD currentBufferSize;  // 現在のバッファサイズ
    
    void fullClear();          // コンソール全体をクリア（起動時）
    void clearGameArea();      // ゲーム描画領域を消去（終了時）
    void restoreConsoleSettings();
    // TODO: ダブルバッファの実装
};
