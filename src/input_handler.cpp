#include "input_handler.h"
#include <conio.h>

// X マクロでキーボタン情報を再定義
#define KEY_BUTTONS(X) \
    X(Left,  left) \
    X(Right, right) \
    X(Shoot, shoot) \
    X(Pause, pause) \
    X(Quit,  quit) \
    X(Enter, enter)

// コンストラクタ：メンバ変数は = false で初期化済みなので本体は空でOK
InputHandler::InputHandler() {}

void InputHandler::poll() {
    // マクロで各メンバ変数をリセット
    #define RESET_MEMBER(CamelCase, snake_case) snake_case##_ = false;
    KEY_BUTTONS(RESET_MEMBER)
    #undef RESET_MEMBER

    while (_kbhit()) {
        int ch = _getch();

        // 拡張キー（方向キー等）は 0 または 0xE0 のプレフィックス
        if (ch == 0 || ch == 0xE0) {
            int ext = _getch();
            if (ext == 75) left_  = true;  // 左矢印
            if (ext == 77) right_ = true;  // 右矢印
        } else {
            switch (ch) {
                case ' ':            shoot_ = true; break;
                case 'p': case 'P':  pause_ = true; break;
                case 'q': case 'Q':  quit_  = true; break;
                case 27:             quit_  = true; break;  // Esc
                case '\r':           enter_ = true; break;
            }
        }
    }
}

// マクロで各種ゲッター関数を生成
#define DEFINE_GETTER(CamelCase, snake_case) \
bool InputHandler::is##CamelCase() const { return snake_case##_; }
KEY_BUTTONS(DEFINE_GETTER)
#undef DEFINE_GETTER

#undef KEY_BUTTONS
