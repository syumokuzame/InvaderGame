#include "bullet.h"
#include "renderer.h"
#include "config.h"

namespace game {

Bullet::Bullet(int x, int y, BulletOwner owner)
    : Actor(x, y),
      dy_(owner == BulletOwner::Player ? -1 : 1),
      active_(true),
      owner_(owner) {}

void Bullet::update() {
    if (!active_) return;
    y_ += dy_;
    // フィールド枠（上枠=UI_HEIGHT+1、下枠=FIELD_HEIGHT-1）に到達したら無効化
    if (y_ <= Config::UI_HEIGHT || y_ >= Config::FIELD_HEIGHT - 1)
        active_ = false;
}

void Bullet::render(Renderer& renderer) const {
    if (!active_) return;
    renderer.draw(x_, y_, '|');
}

void Bullet::deactivate() {
    active_ = false;
}

bool Bullet::isActive() const {
    return active_;
}

}  // namespace game
