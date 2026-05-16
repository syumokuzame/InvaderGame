#pragma once

#include "../../Engine/include/SceneBase.h"
#include "../input_handler.h"

namespace game {

class TitleScene : public engine::SceneBase {
public:
    TitleScene();

    void calc() override;
    void draw(engine::Renderer& renderer) override;

private:
    InputHandler input_;
    int          frameCount_     = 0;   // 点滅エフェクト用
    int          inputCooldown_  = 30;  // 入力受け付けクールダウン（フレーム）
};

}  // namespace game
