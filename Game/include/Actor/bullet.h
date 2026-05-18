#pragma once

#include "Actor/actor.h"

enum class BulletOwner { Player, Enemy };

namespace game {

class Bullet : public Actor {
public:
    Bullet(int x, int y, BulletOwner owner);

    void preCalc() override;
    bool isActive() const override;

    // 弾は Bullet レイヤーで描画
    int drawLayer() const override {
        return static_cast<int>(engine::RenderLayer::Bullet);
    }

    BulletOwner owner() const { return mOwner; }

    void deactivate_();

private:
    int         mDy;   // 移動方向（プレイヤー: -1, 敵: +1）
    bool        mActive;
    BulletOwner mOwner;
};

}  // namespace game
