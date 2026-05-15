#include "bullet.h"
#include "renderer.h"
#include "config.h"

Bullet::Bullet(int x, int y, BulletOwner owner)
    : x_(x), y_(y),
      dy_(owner == BulletOwner::Player ? -1 : 1),
      active_(true),
      owner_(owner) {}

void Bullet::update() {
    if (!active_) return;
    y_ += dy_;
    // フィールド枠に到達したら無効化
    if (y_ <= 0 || y_ >= Config::FIELD_HEIGHT - 1)
        active_ = false;
}

void Bullet::render(Renderer& renderer) const {
    if (!active_) return;
    renderer.draw(x_, y_, '|');
}

void Bullet::deactivate() {
    active_ = false;
}
