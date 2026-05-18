#pragma once

#include "../../Engine/include/SceneBase.h"
#include "../../Engine/include/InputHandler.h"

namespace game {

class TitleScene : public engine::SceneBase {
public:
    explicit TitleScene(engine::Allocator& allocator);

    void calc() override;
    void draw() override;

private:
    engine::InputHandler mInput;
    int          mFrameCount     = 0;   // 点滅エフェクト用
    int          mInputCooldown  = 30;  // 入力受け付けクールダウン（フレーム）
};

}  // namespace game
