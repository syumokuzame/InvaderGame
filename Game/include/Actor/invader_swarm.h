#pragma once

#include "Actor/invader.h"
#include "config.h"
#include <list>
#include <vector>

namespace game {

class Bullet;

class InvaderSwarm : public engine::ActorBase {
public:
    // bullets: GameScene が所有する弾リストへの参照（postCalc で当たり判定に使用）
    explicit InvaderSwarm(std::list<Bullet>& bullets);

    void reset_(int level);    // レベルに応じて速度・頻度を設定

    // Phase 1: 各インベーダーのアニメ更新 + 群れの移動
    void preCalc()  override;

    // Phase 2: プレイヤー弾との当たり判定（弾の位置確定後）
    void postCalc() override;

    // 内包する全インベーダーを描画する
    void draw()     const override;

    bool isActive() const override { return true; }

    bool allDefeated_() const;
    bool hasReachedBottom_() const;

    std::vector<Invader>& invaders() { return mInvaders; }

private:
    std::list<Bullet>& mBullets_;   // 非所有参照

    std::vector<Invader> mInvaders;
    int  mDx;          // 横移動方向 (+1 or -1)
    int  mSpeed;       // 移動間隔（フレーム数）
    int  mTimer;
    int  mShootInterval;
    int  mShootTimer;

    void moveDown_();
};

}  // namespace game
