#pragma once

#include "Actor/invader.h"
#include "config.h"
#include <list>
#include <vector>

namespace game {

class InvaderSwarm : public engine::ActorBase {
public:
    InvaderSwarm();

    void reset_(int level);    // レベルに応じて速度・頻度を設定

    // Phase 1: 各インベーダーのアニメ更新 + 群れの移動
    void preCalc()  override;

    // Phase 2: ColliderComponent の衝突結果を読んで各インベーダーを kill_()
    void postCalc() override;

    // 内包する全インベーダーを描画する
    void draw()     const override;

    bool isActive() const override { return true; }

    bool allDefeated_() const;
    bool hasReachedBottom_() const;

    std::list<Invader>& invaders() { return mInvaders; }

    // isFullyDead_() な Invader を mInvaders から消去する
    // cleanupColliders_() の後に呼ぶこと（ポインタの安全な解放のため）
    void sweepDead_();

private:
    std::list<Invader> mInvaders;  // std::list で要素アドレスを安定させる
    int  mDx;          // 横移動方向 (+1 or -1)
    int  mSpeed;       // 移動間隔（フレーム数）
    int  mTimer;
    int  mShootInterval;
    int  mShootTimer;

    void moveDown_();
};

}  // namespace game
