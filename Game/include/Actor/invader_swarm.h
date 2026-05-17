#pragma once

#include "Actor/invader.h"
#include "config.h"
#include <vector>

namespace game {

class Bullet;

class InvaderSwarm {
public:
    InvaderSwarm();

    void reset_(int level);    // レベルに応じて速度・頻度を設定
    void update_(std::vector<Bullet>& bullets);
    void draw_() const;

    bool allDefeated_() const;
    bool hasReachedBottom_() const;

    std::vector<Invader>& invaders() { return mInvaders; }

private:
    std::vector<Invader> mInvaders;
    int  mDx;          // 横移動方向 (+1 or -1)
    int  mSpeed;       // 移動間隔（フレーム数）
    int  mTimer;
    int  mShootInterval;
    int  mShootTimer;

    void moveDown_();
};

}  // namespace game
