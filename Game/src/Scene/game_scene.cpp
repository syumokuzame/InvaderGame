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

    // Actor を登録（弾は生成時に registerActor_ する）
    registerActor_(mPlayer);
    registerActor_(&mSwarm);

    // Invader を衝突走査リストに登録（mActors_ ではなく mColliders_ へ）
    for (auto& inv : mSwarm.invaders()) {
        registerCollider_(&inv);
    }

    // 前シーンからのキー入力状態をリセット
    mInput.poll_();
    mGameStartTime = std::time(nullptr);
    for (const auto& inv : mSwarm.invaders()) {
        if (inv.isAlive_()) mLastAliveCount++;  // 生存中のインベーダーのみカウント
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
            registerActor_(&mBullets.back());  // 生成した弾を mActors_ に登録
        }
    }
    if (mInput.isDebugKillAll_()) {
        debugKillAllInvaders_();
    }
}

void GameScene::sceneCalcImpl_() {
    // 前フレームの Game 固有後処理
    // （SceneBase::calc() が cleanupActors_/cleanupColliders_ を完了した後の状態で実行される）
    // 完全死亡インベーダーを mColliders_ 除去後に mInvaders から消去する
    mSwarm.sweepDead_();

    // 非アクティブ弾を mBullets リストから除去
    for (auto it = mBullets.begin(); it != mBullets.end(); ) {
        if (!it->isActive()) {
            if (it->owner() == BulletOwner::Player)
                mPlayer->clearBullet_();
            it = mBullets.erase(it);
        } else {
            ++it;
        }
    }

    // 入力処理
    processInput_();

    // シーンのロジック計算（前フレーム postCalc 後の Actor 状態を参照）
    if (mState == GameState::Playing) {
        int currentAliveCount = 0;
        for (const auto& inv : mSwarm.invaders()) {
            if (inv.isAlive_()) currentAliveCount++;  // ← isAlive_() を使用（消滅アニメ中は除外）
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
            // リセット前に古い Invader のコリジョン登録を全消去
            clearColliders_();
            mSwarm.reset_(mLevel);
            // 新しい Invader を衝突走査リストに再登録
            for (auto& inv : mSwarm.invaders()) {
                registerCollider_(&inv);
            }
            mLastAliveCount = 0;
            for (const auto& inv : mSwarm.invaders()) {
                if (inv.isAlive_()) mLastAliveCount++;  // 生存中のインベーダーのみカウント
            }
            // 全弾を非アクティブ化（cleanup で mActors_ と mBullets の両方から除去される）
            for (auto& b : mBullets) b.deactivate_();
            mPlayer->clearBullet_();
            mClearMessageUI->setVisible(false);
            mState = GameState::Playing;
        }
    }
}

void GameScene::draw() {
    // 背景を描画（枠線はEngine側で処理）
    drawBackground_(Config::FIELD_WIDTH, Config::FIELD_HEIGHT);

    // 全アクターを一括描画（RenderQueue がレイヤー順に整列）
    drawActors_();

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
