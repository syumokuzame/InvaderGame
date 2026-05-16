#include "Scene/title_scene.h"
#include "Scene/game_scene.h"
#include "Renderer.h"
#include "config.h"

namespace game {

TitleScene::TitleScene() {
    // 前シーンからのキー入力状態をリセット
    input_.poll();
}

void TitleScene::calc() {
    input_.poll();

    // クールダウン中は入力を受け付けない
    if (inputCooldown_ > 0) {
        --inputCooldown_;
        ++frameCount_;
        return;
    }

    if (input_.isQuit()) {
        quit();
        return;
    }

    if (input_.isEnter()) {
        changeScene(new GameScene());
        return;
    }

    ++frameCount_;
}

void TitleScene::draw(engine::Renderer& renderer) {
    const int cx = Config::FIELD_WIDTH / 2;
    const int cy = Config::FIELD_HEIGHT / 2;

    renderer.drawString(cx - 6, cy - 4, "INVADER GAME");
    renderer.drawString(cx - 9, cy - 2, "======================");

    // Press ENTER 点滅（60フレームサイクル）
    if ((frameCount_ / 30) % 2 == 0) {
        renderer.drawString(cx - 9, cy, "  Press ENTER to Start  ");
    }

    renderer.drawString(cx - 9, cy + 2, " Press Q / ESC to Quit  ");
}

}  // namespace game
