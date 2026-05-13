#pragma once

// ダブルバッファリングを用いた描画管理
class Renderer {
public:
    Renderer();
    ~Renderer();

    void clear();           // バックバッファをクリア
    void draw(int x, int y, char ch);   // 指定座標に文字を描画
    void drawString(int x, int y, const char* str);
    void present();         // バックバッファをフロントに転送（描画確定）

    void hideCursor();
    void showCursor();

private:
    // TODO: ダブルバッファの実装
};
