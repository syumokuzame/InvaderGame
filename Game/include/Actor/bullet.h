#pragma once

#include "Actor/actor.h"
#include "ColliderComponent.h"

enum class BulletOwner { Player, Enemy };

namespace game {

class Bullet : public Actor {
public:
    Bullet(int x, int y, BulletOwner owner);

    void preCalc()  override;
    void postCalc() override;  // 衝突結果を受けて非アクティブ化
    bool isActive() const override;

    // 弾は Bullet レイヤーで描画
    int drawLayer() const override {
        return static_cast<int>(engine::RenderLayer::Bullet);
    }

    BulletOwner owner() const { return mOwner; }

    void deactivate_();

    // 当たり判定コンポーネント（mActors_ 経由で SceneBase が collision 走査に使用）
    engine::ColliderComponent* collider() override { return &mCollider_; }

private:
    int         mDy;    // 移動方向（プレイヤー: -1, 敵: +1）
    bool        mActive;
    BulletOwner mOwner;
    engine::ColliderComponent mCollider_;
};

}  // namespace game
