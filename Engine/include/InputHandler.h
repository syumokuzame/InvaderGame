#pragma once
#include <functional>

namespace engine {

// X マクロでボタン情報を定義
// Game 固有のボタン名を含むが、この具体的ハンドラーはゲーム専用のため許容する
#define INPUT_BUTTONS(X) \
    X(Left,         left) \
    X(Right,        right) \
    X(Shoot,        shoot) \
    X(Pause,        pause) \
    X(Quit,         quit) \
    X(Enter,        enter) \
    X(DebugKillAll, debugKillAll)

// キー入力の非ブロッキング取得 (Windows Console API)
//
// テスト時は setTestProvider() でコールバックを登録することで
// 実際のキーボード入力を差し替えられる。
// コールバックを nullptr にすると通常入力モードに戻る。
class InputHandler {
public:
    // TestProvider: (フレーム番号, InputHandler参照) を受け取り
    //               set##Xxx##_() で任意のボタン状態を注入する
    using TestProvider = std::function<void(int frame, InputHandler& input)>;

    InputHandler();

    // テストプロバイダーを登録する（nullptr で解除 → 通常入力に戻る）
    // プロバイダー登録時にフレームカウンタはリセットされる
    static void setTestProvider(TestProvider provider);

    void poll_();  // 入力状態を更新

    // ゲッター（マクロで生成）
    #define DECLARE_GETTER(CamelCase, snake_case) bool is##CamelCase##_() const;
    INPUT_BUTTONS(DECLARE_GETTER)
    #undef DECLARE_GETTER

    // テスト用セッター（TestProvider コールバック内から使用）
    #define DECLARE_SETTER(CamelCase, snake_case) void set##CamelCase##_(bool v);
    INPUT_BUTTONS(DECLARE_SETTER)
    #undef DECLARE_SETTER

private:
    static TestProvider sTestProvider;
    static int          sTestFrame;

    // ボタン状態（= false で初期化済み）
    #define DECLARE_MEMBER(CamelCase, snake_case) bool m##CamelCase = false;
    INPUT_BUTTONS(DECLARE_MEMBER)
    #undef DECLARE_MEMBER
};

}  // namespace engine

#undef INPUT_BUTTONS
