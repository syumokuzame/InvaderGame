#pragma once

namespace game {

// X マクロでキーボタン情報を定義
#define KEY_BUTTONS(X) \
    X(Left,  left) \
    X(Right, right) \
    X(Shoot, shoot) \
    X(Pause, pause) \
    X(Quit,  quit) \
    X(Enter, enter) \
    X(DebugKillAll, debugKillAll)

// キー入力の非ブロッキング取得 (Windows Console API)
class InputHandler {
public:
    InputHandler();

    void poll_();   // 入力状態を更新

    // マクロでゲッター関数を宣言
    #define DECLARE_GETTER(CamelCase, snake_case) bool is##CamelCase##_() const;
    KEY_BUTTONS(DECLARE_GETTER)
    #undef DECLARE_GETTER

private:
    // マクロでメンバ変数を生成（= false で初期化）
    #define DECLARE_MEMBER(CamelCase, snake_case) bool m##CamelCase = false;
    KEY_BUTTONS(DECLARE_MEMBER)
    #undef DECLARE_MEMBER
};

}  // namespace game

#undef KEY_BUTTONS
