#include "InputHandler.h"
#include <windows.h>

// .h で #undef されているため .cpp 内で再定義する
#define INPUT_BUTTONS(X) \
    X(Left,         left) \
    X(Right,        right) \
    X(Shoot,        shoot) \
    X(Pause,        pause) \
    X(Quit,         quit) \
    X(Enter,        enter) \
    X(DebugKillAll, debugKillAll)

namespace engine {

InputHandler::TestProvider InputHandler::sTestProvider = nullptr;
int                        InputHandler::sTestFrame    = 0;

void InputHandler::setTestProvider(TestProvider provider) {
    sTestProvider = std::move(provider);
    sTestFrame    = 0;
}

InputHandler::InputHandler() {}

void InputHandler::poll_() {
    // 全ボタンをリセット
    #define RESET_MEMBER(CamelCase, snake_case) m##CamelCase = false;
    INPUT_BUTTONS(RESET_MEMBER)
    #undef RESET_MEMBER

    if (sTestProvider) {
        ++sTestFrame;
        sTestProvider(sTestFrame, *this);
        return;
    }

    // 実キーボード入力
    if (GetAsyncKeyState(VK_LEFT)   & 0x8000) mLeft         = true;
    if (GetAsyncKeyState(VK_RIGHT)  & 0x8000) mRight        = true;
    if (GetAsyncKeyState(VK_SPACE)  & 0x8000) mShoot        = true;
    if (GetAsyncKeyState('P')       & 0x8000) mPause        = true;
    if (GetAsyncKeyState('Q')       & 0x8000) mQuit         = true;
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) mQuit         = true;
    if (GetAsyncKeyState(VK_RETURN) & 0x8000) mEnter        = true;
    if (GetAsyncKeyState('A')       & 0x8000) mDebugKillAll = true;
}

#define DEFINE_GETTER(CamelCase, snake_case) \
bool InputHandler::is##CamelCase##_() const { return m##CamelCase; }
INPUT_BUTTONS(DEFINE_GETTER)
#undef DEFINE_GETTER

#define DEFINE_SETTER(CamelCase, snake_case) \
void InputHandler::set##CamelCase##_(bool v) { m##CamelCase = v; }
INPUT_BUTTONS(DEFINE_SETTER)
#undef DEFINE_SETTER

}  // namespace engine

#undef INPUT_BUTTONS
