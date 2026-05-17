#include "Scene/game_scene.h"
#include "Scene/title_scene.h"
#include "Renderer.h"
#include "config.h"
#include <algorithm>
#include <ctime>

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

void GameScene::draw(engine::Renderer& renderer) {
    std::time_t now = std::time(nullptr);
    int elapsedSeconds = static_cast<int>(now - mGameStartTime);
    renderer.drawHeader("InvaderGame", mScoreManager.score_(), elapsedSeconds);
    renderer.drawInstructions();

    for (int x = 0; x < Config::FIELD_WIDTH; ++x) {
        renderer.draw(x, 2, '=');
        renderer.draw(x, Config::FIELD_HEIGHT - 1, '=');
    }
    for (int y = 3; y < Config::FIELD_HEIGHT - 1; ++y) {
        renderer.draw(0, y, '|');
        renderer.draw(Config::FIELD_WIDTH - 1, y, '|');
    }

    for (const auto& b : mBullets)
        b.draw(renderer);

    mSwarm.draw_(renderer);
    mPlayer->draw(renderer);

    if (mState == GameState::GameClear) {
        int msgY = Config::FIELD_HEIGHT / 2;
        int msgX = Config::FIELD_WIDTH / 2 - 2;
        renderer.drawString(msgX, msgY, "CLEAR!");
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
