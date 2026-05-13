#pragma once

// キー入力の非ブロッキング取得 (Windows Console API)
class InputHandler {
public:
    InputHandler();

    void poll();   // 入力状態を更新

    bool isLeft()  const;
    bool isRight() const;
    bool isShoot() const;
    bool isPause() const;
    bool isQuit()  const;
    bool isEnter() const;

private:
    bool left_, right_, shoot_, pause_, quit_, enter_;
};
