#include "Actor/bullet.h"
#include "config.h"

namespace game {

Bullet::Bullet(int x, int y, BulletOwner owner)
    : Actor(x, y),
      mDy(owner == BulletOwner::Player ? -1 : 1),
      mActive(true),
      mOwner(owner) {}

void Bullet::calc() {
    if (!mActive) {
        mModel.cells = {};  // 非アクティブ時はモデルを空に
        return;
    }
    mY += mDy;
    // フィールド枠（上枠=UI_HEIGHT+1、下枠=FIELD_HEIGHT-1）に到達したら無効化
    if (mY <= Config::UI_HEIGHT || mY >= Config::FIELD_HEIGHT - 1) {
        mActive = false;
        mModel.cells = {};
        return;
    }
    // モデル更新（pivot=(0,0) = mX,mY が '|' の位置）
    mModel.cells  = { "|" };
    mModel.pivotX = 0;
    mModel.pivotY = 0;
}

void Bullet::deactivate_() {
    mActive = false;
}

bool Bullet::isActive() const {
    return mActive;
}

}  // namespace game
