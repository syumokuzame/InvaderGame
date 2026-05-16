#pragma once

#include "actor.h"

class Renderer;

enum class BulletOwner { Player, Enemy };

namespace game {

class Bullet : public Actor {
public:
    Bullet(int x, int y, BulletOwner owner);

    void update() override;
    void render(Renderer& renderer) const override;
    bool isActive() const override;

    BulletOwner owner() const { return owner_; }

    void deactivate();

private:
    int         dy_;   // 移動方向（プレイヤー: -1, 敵: +1）
    bool        active_;
    BulletOwner owner_;
};

}  // namespace game
