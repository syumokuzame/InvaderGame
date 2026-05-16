#pragma once

#include "Actor/actor.h"

enum class BulletOwner { Player, Enemy };

namespace game {

class Bullet : public Actor {
public:
    Bullet(int x, int y, BulletOwner owner);

    void calc() override;
    void draw(engine::Renderer& renderer) const override;
    bool isActive() const override;

    BulletOwner owner() const { return owner_; }

    void deactivate();

private:
    int         dy_;   // 移動方向（プレイヤー: -1, 敵: +1）
    bool        active_;
    BulletOwner owner_;
};

}  // namespace game
