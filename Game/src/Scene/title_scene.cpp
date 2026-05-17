#include "Scene/title_scene.h"
#include "Scene/game_scene.h"
#include "RenderQueue.h"
#include "config.h"
#include "Logger.h"

namespace game {

TitleScene::TitleScene(engine::Allocator& allocator)
    : engine::SceneBase(allocator) {
    // 前シーンからのキー入力状態をリセット
    mInput.poll_();
    engine::Logger::instance().log("[SCENE] TitleScene started");
}

void TitleScene::calc() {
    mInput.poll_();

    // クールダウン中は入力を受け付けない
    if (mInputCooldown > 0) {
        --mInputCooldown;
        ++mFrameCount;
        return;
    }

    if (mInput.isQuit_()) {
        engine::Logger::instance().log("[INPUT] Quit pressed -> game exit");
        quit();
        return;
    }

    if (mInput.isEnter_()) {
        engine::Logger::instance().log("[INPUT] Enter pressed -> GameScene");
        changeScene(engine::SceneType::Game);
        return;
    }

    ++mFrameCount;
}

void TitleScene::draw() {
    auto& rq = engine::RenderQueue::instance();
    const int uiLayer = static_cast<int>(engine::RenderLayer::UI);
    const int cx = Config::FIELD_WIDTH / 2;
    const int cy = Config::FIELD_HEIGHT / 2;

    rq.submitString(cx - 6, cy - 4, "INVADER GAME", uiLayer);
    rq.submitString(cx - 9, cy - 2, "======================", uiLayer);

    // Press ENTER 点滅（60フレームサイクル）
    if ((mFrameCount / 30) % 2 == 0) {
        rq.submitString(cx - 9, cy, "  Press ENTER to Start  ", uiLayer);
    }

    rq.submitString(cx - 9, cy + 2, " Press Q / ESC to Quit  ", uiLayer);
}

}  // namespace game
