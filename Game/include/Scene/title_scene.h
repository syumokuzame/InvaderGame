#pragma once

#include "../../Engine/include/SceneBase.h"
#include "../input_handler.h"

namespace game {

class TitleScene : public engine::SceneBase {
public:
    explicit TitleScene(engine::Allocator& allocator);

    void calc() override;
    void draw(engine::Renderer& renderer) override;

private:
    InputHandler mInput;
    int          mFrameCount     = 0;   // 点滅エフェクト用
    int          mInputCooldown  = 30;  // 入力受け付けクールダウン（フレーム）
};

}  // namespace game
