#include "input_handler.h"
#include <windows.h>

namespace game {

bool InputHandler::sTestMode  = false;
int  InputHandler::sTestFrame = 0;

void InputHandler::enableTestMode() {
    sTestMode = true;
}

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

void InputHandler::poll_() {
    // マクロで各メンバ変数をリセット
    #define RESET_MEMBER(CamelCase, snake_case) m##CamelCase = false;
    KEY_BUTTONS(RESET_MEMBER)
    #undef RESET_MEMBER

    if (sTestMode) {
        // フレームカウンタを進めてスクリプト入力を再生する
        // 各シーンのコンストラクタでも poll_() が呼ばれるため、閾値にさらに余裕を持たせてある
        ++sTestFrame;
        //  65: Titleクールダウン(30f) + 待機(35f) → Enter で GameScene へ
        // 130: Gameクールダウン(30f) + 待機(34f) → 射撃
        // 145: デバッグ全滅 → スコア加算検証用
        // 160: Qキー → TitleScene へ戻る
        // 260: Qキー → ゲーム終了、exe自動終了
        if      (sTestFrame ==  65) mEnter        = true;
        else if (sTestFrame == 130) mShoot        = true;
        else if (sTestFrame == 145) mDebugKillAll = true;
        else if (sTestFrame == 160) mQuit         = true;
        else if (sTestFrame == 260) mQuit         = true;
        return;
    }

    // GetAsyncKeyState() で各キーの状態を確認
    // 戻り値の最上位ビットが 1 の場合、キーが押されている
    if (GetAsyncKeyState(VK_LEFT)  & 0x8000) mLeft  = true;
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) mRight = true;
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) mShoot = true;
    if (GetAsyncKeyState('P')      & 0x8000) mPause = true;
    if (GetAsyncKeyState('Q')      & 0x8000) mQuit  = true;
    if (GetAsyncKeyState(VK_ESCAPE)& 0x8000) mQuit  = true;
    if (GetAsyncKeyState(VK_RETURN)& 0x8000) mEnter = true;
    if (GetAsyncKeyState('A')      & 0x8000) mDebugKillAll = true;
}

// マクロで各種ゲッター関数を生成
#define DEFINE_GETTER(CamelCase, snake_case) \
bool InputHandler::is##CamelCase##_() const { return m##CamelCase; }
KEY_BUTTONS(DEFINE_GETTER)
#undef DEFINE_GETTER

#undef KEY_BUTTONS

}  // namespace game
