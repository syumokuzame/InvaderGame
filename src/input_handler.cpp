#include "input_handler.h"
#include <conio.h>

InputHandler::InputHandler()
    : left_(false), right_(false), shoot_(false),
      pause_(false), quit_(false), enter_(false) {}

void InputHandler::poll() {
    left_ = right_ = shoot_ = pause_ = quit_ = enter_ = false;

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

bool InputHandler::isLeft()  const { return left_;  }
bool InputHandler::isRight() const { return right_; }
bool InputHandler::isShoot() const { return shoot_; }
bool InputHandler::isPause() const { return pause_; }
bool InputHandler::isQuit()  const { return quit_;  }
bool InputHandler::isEnter() const { return enter_; }
