#include "Scene/game_scene.h"
#include "Scene/title_scene.h"
#include "RenderQueue.h"
#include "config.h"
#include <algorithm>
#include <ctime>
#include <cstdio>

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
}

void GameScene::processInput_() {
    mInput.poll_();

    // クールダウン中は入力を受け付けない
    if (mInputCooldown > 0) {
        --mInputCooldown;
        return;
    }

    if (mInput.isQuit_())  { changeScene(engine::SceneType::Title); return; }
    if (mInput.isLeft_())  mPlayer->moveLeft_();
    if (mInput.isRight_()) mPlayer->moveRight_();
    if (mInput.isShoot_()) {
        if (mPlayer->shoot_()) {
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
            mScoreManager.addScore_(defeatedCount * 10);
        }
        mLastAliveCount = currentAliveCount;

        if (mSwarm.allDefeated_()) {
            mState = GameState::GameClear;
            mClearCounter = 0;
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
            mState = GameState::Playing;
        }
    }

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
    const int uiLayer = static_cast<int>(engine::RenderLayer::UI);
    const int bgLayer = static_cast<int>(engine::RenderLayer::Background);

    // ヘッダー（UI レイヤー）
    std::time_t now = std::time(nullptr);
    int elapsedSeconds = static_cast<int>(now - mGameStartTime);
    rq.submitString(1, 0, "InvaderGame", uiLayer);

    char scoreBuf[32];
    sprintf_s(scoreBuf, sizeof(scoreBuf), "Score: %d", mScoreManager.score_());
    rq.submitString(Config::FIELD_WIDTH - 30, 0, scoreBuf, uiLayer);

    char timeBuf[16];
    int minutes = elapsedSeconds / 60;
    int seconds  = elapsedSeconds % 60;
    sprintf_s(timeBuf, sizeof(timeBuf), "%02d:%02d", minutes, seconds);
    rq.submitString(Config::FIELD_WIDTH - 8, 0, timeBuf, uiLayer);

    rq.submitString(2, 1, "LEFT/RIGHT: Move  SPACE: Shoot  P: Pause  Q: Quit", uiLayer);

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

    if (mState == GameState::GameClear) {
        int msgY = Config::FIELD_HEIGHT / 2;
        int msgX = Config::FIELD_WIDTH / 2 - 2;
        rq.submitString(msgX, msgY, "CLEAR!", uiLayer);
    }
}

void GameScene::debugKillAllInvaders_() {
    for (auto& inv : mSwarm.invaders()) {
        if (!inv.isFullyDead_()) {
            inv.kill_();
        }
    }
}

}  // namespace game
