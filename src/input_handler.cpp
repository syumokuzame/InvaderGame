#include "input_handler.h"
#include <windows.h>

// X マクロでキーボタン情報を再定義
#define KEY_BUTTONS(X) \
    X(Left,  left) \
    X(Right, right) \
    X(Shoot, shoot) \
    X(Pause, pause) \
    X(Quit,  quit) \
    X(Enter, enter) \
    X(DebugKillAll, debugKillAll)

// コンストラクタ：メンバ変数は = false で初期化済みなので本体は空でOK
InputHandler::InputHandler() {}

void InputHandler::poll() {
    // マクロで各メンバ変数をリセット
    #define RESET_MEMBER(CamelCase, snake_case) snake_case##_ = false;
    KEY_BUTTONS(RESET_MEMBER)
    #undef RESET_MEMBER

    // GetAsyncKeyState() で各キーの状態を確認
    // 戻り値の最上位ビットが 1 の場合、キーが押されている
    if (GetAsyncKeyState(VK_LEFT)  & 0x8000) left_  = true;
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) right_ = true;
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) shoot_ = true;
    if (GetAsyncKeyState('P')      & 0x8000) pause_ = true;
    if (GetAsyncKeyState('Q')      & 0x8000) quit_  = true;
    if (GetAsyncKeyState(VK_ESCAPE)& 0x8000) quit_  = true;
    if (GetAsyncKeyState(VK_RETURN)& 0x8000) enter_ = true;
    if (GetAsyncKeyState('A')      & 0x8000) debugKillAll_ = true;
}

// マクロで各種ゲッター関数を生成
#define DEFINE_GETTER(CamelCase, snake_case) \
bool InputHandler::is##CamelCase() const { return snake_case##_; }
KEY_BUTTONS(DEFINE_GETTER)
#undef DEFINE_GETTER

#undef KEY_BUTTONS
