#include "Scene/game_scene.h"
#include "Scene/title_scene.h"
#include "RenderQueue.h"
#include "config.h"
#include "Logger.h"
#include <algorithm>
#include <ctime>
#include <string>
namespace game {

GameScene::GameScene(engine::Allocator& allocator)
    : engine::SceneBase(allocator),
      mState(GameState::Playing), mLevel(1), mClearCounter(0), mLastAliveCount(0) {
    // Player を Allocator 経由でヒープ確保（解放は Allocator が担う）
    mPlayer = mAllocator.create<Player>();

    // 前シーンからのキー入力状態をリセット
    mInput.poll_();
    mGameStartTime = std::time(nullptr);
    for (const auto& inv : mSwarm.invaders()) {
        if (inv.isActive()) mLastAliveCount++;
    }

    // UI 登録（所有権は SceneBase::mUIs_ が管理）
    addUI_(std::make_unique<engine::UIBase>(1, 0, "InvaderGame"));
    addUI_(std::make_unique<engine::UIBase>(
        2, 1, "LEFT/RIGHT: Move  SPACE: Shoot  P: Pause  Q: Quit"));
    addUI_(std::make_unique<ScoreUI>(Config::FIELD_WIDTH - 30, 0, mScoreManager));
    addUI_(std::make_unique<TimerUI>(Config::FIELD_WIDTH - 8, 0, mGameStartTime));

    const int msgX = Config::FIELD_WIDTH / 2 - 3;
    const int msgY = Config::FIELD_HEIGHT / 2;
    mClearMessageUI = addUI_(std::make_unique<engine::UIBase>(msgX, msgY, "CLEAR!"));
    mClearMessageUI->setVisible(false);

    engine::Logger::instance().log(
        "[SCENE] GameScene started, invaders=" + std::to_string(mLastAliveCount));
}

void GameScene::processInput_() {
    mInput.poll_();

    // クールダウン中は入力を受け付けない
    if (mInputCooldown > 0) {
        --mInputCooldown;
        return;
    }

    if (mInput.isQuit_())  {
        engine::Logger::instance().log("[INPUT] Quit pressed -> TitleScene");
        changeScene(engine::SceneType::Title); return;
    }
    if (mInput.isLeft_())  mPlayer->moveLeft_();
    if (mInput.isRight_()) mPlayer->moveRight_();
    if (mInput.isShoot_()) {
        if (mPlayer->shoot_()) {
            engine::Logger::instance().log("[INPUT] Player shoot");
            mBullets.emplace_back(mPlayer->x(), mPlayer->y() - 1, BulletOwner::Player);
        }
    }
    if (mInput.isDebugKillAll_()) {
        debugKillAllInvaders_();
    }
}

void GameScene::calc() {
    processInput_();

    mPlayer->calc();

    for (auto& b : mBullets) {
        b.calc();
    }

    if (mState == GameState::Playing) {
        mSwarm.update_(mBullets);

        int currentAliveCount = 0;
        for (const auto& inv : mSwarm.invaders()) {
            if (inv.isActive()) currentAliveCount++;
        }
        int defeatedCount = mLastAliveCount - currentAliveCount;
        if (defeatedCount > 0) {
            engine::Logger::instance().log(
                "[SCORE] Added " + std::to_string(defeatedCount * 10) +
                " (total=" + std::to_string(mScoreManager.score_()) + ")");
            mScoreManager.addScore_(defeatedCount * 10);
        }
        mLastAliveCount = currentAliveCount;

        if (mSwarm.allDefeated_()) {
            mState = GameState::GameClear;
            mClearCounter = 0;
            mClearMessageUI->setVisible(true);
        }
    } else if (mState == GameState::GameClear) {
        mClearCounter++;
        if (mClearCounter >= 60) {
            mSwarm.reset_(mLevel);
            mLastAliveCount = 0;
            for (const auto& inv : mSwarm.invaders()) {
                if (inv.isActive()) mLastAliveCount++;
            }
            mBullets.clear();
            mPlayer->clearBullet_();
            mClearMessageUI->setVisible(false);
            mState = GameState::Playing;
        }
    }

    calcUIs_();

    mBullets.erase(
        std::remove_if(mBullets.begin(), mBullets.end(),
            [this](const Bullet& b) {
                if (!b.isActive()) {
                    if (b.owner() == BulletOwner::Player)
                        mPlayer->clearBullet_();
                    return true;
                }
                return false;
            }),
        mBullets.end()
    );
}

void GameScene::draw() {
    auto& rq = engine::RenderQueue::instance();
    const int bgLayer = static_cast<int>(engine::RenderLayer::Background);

    // 枠線（Background レイヤー）
    for (int x = 0; x < Config::FIELD_WIDTH; ++x) {
        rq.submit(x, 2, '=', bgLayer);
        rq.submit(x, Config::FIELD_HEIGHT - 1, '=', bgLayer);
    }
    for (int y = 3; y < Config::FIELD_HEIGHT - 1; ++y) {
        rq.submit(0, y, '|', bgLayer);
        rq.submit(Config::FIELD_WIDTH - 1, y, '|', bgLayer);
    }

    // Actor（Actor / Bullet レイヤー）
    for (const auto& b : mBullets)
        b.draw();

    mSwarm.draw_();
    mPlayer->draw();

    // UI（UIBase サブクラスを一括描画）
    drawUIs_();
}

void GameScene::debugKillAllInvaders_() {
    for (auto& inv : mSwarm.invaders()) {
        if (!inv.isFullyDead_()) {
            inv.kill_();
        }
    }
}

}  // namespace game
