#include "Actor/bullet.h"
#include "Renderer.h"
#include "config.h"

namespace game {

Bullet::Bullet(int x, int y, BulletOwner owner)
    : Actor(x, y),
      mDy(owner == BulletOwner::Player ? -1 : 1),
      mActive(true),
      mOwner(owner) {}

void Bullet::calc() {
    if (!mActive) return;
    mY += mDy;
    // フィールド枠（上枠=UI_HEIGHT+1、下枠=FIELD_HEIGHT-1）に到達したら無効化
    if (mY <= Config::UI_HEIGHT || mY >= Config::FIELD_HEIGHT - 1)
        mActive = false;
}

void Bullet::draw(engine::Renderer& renderer) const {
    if (!mActive) return;
    renderer.draw(mX, mY, '|');
}

void Bullet::deactivate_() {
    mActive = false;
}

bool Bullet::isActive() const {
    return mActive;
}

}  // namespace game
