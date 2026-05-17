#include "Actor/player.h"
#include "config.h"

namespace game {

Player::Player()
    : Actor(Config::FIELD_WIDTH / 2, Config::FIELD_HEIGHT - 2),
      mLives(Config::PLAYER_LIVES),
      mHasBullet(false),
      mShootFrame(0),
      mMoveFrame(0),
      mLastDirection(0) {}

void Player::moveLeft_() {
    if (mX > 2) {
        --mX;
        if (mLastDirection != -1) {
            // 移動方向が左に変わったのでアニメーションをリセット
            mMoveFrame = 15;
            mLastDirection = -1;
        } else if (mMoveFrame == 0) {
            // 移動継続中で、アニメーションが終わっていればリセット
            mMoveFrame = 15;
        }
    }
}

void Player::moveRight_() {
    if (mX < Config::FIELD_WIDTH - 3) {
        ++mX;
        if (mLastDirection != 1) {
            // 移動方向が右に変わったのでアニメーションをリセット
            mMoveFrame = 15;
            mLastDirection = 1;
        } else if (mMoveFrame == 0) {
            // 移動継続中で、アニメーションが終わっていればリセット
            mMoveFrame = 15;
        }
    }
}

bool Player::shoot_() {
    if (mHasBullet) return false;  // すでに弾が飛んでいる
    mHasBullet = true;
    mShootFrame = 10;  // 射撃アニメーション開始
    return true;
}

void Player::clearBullet_() {
    mHasBullet = false;
}

void Player::calc() {
    // 射撃アニメーションカウンターをデクリメント
    if (mShootFrame > 0) {
        --mShootFrame;
    }
    
    // 移動アニメーションカウンターをデクリメント
    if (mMoveFrame > 0) {
        --mMoveFrame;
    }

    // アニメーション状態に応じてモデルを更新
    // pivot は常に (1, 0) = mX が '^' や '|' の位置
    if (mShootFrame > 0) {
        // 射撃中：中央が '|'
        mModel.cells  = { "<|>" };
        mModel.pivotX = 1;
        mModel.pivotY = 0;
    } else if (mMoveFrame > 0 && mLastDirection == -1) {
        // 左移動中：右側に噴射炎
        //   pivot=1 → mX が '^' の位置、右に ">>." が続く
        mModel.cells  = { "<^>>." };
        mModel.pivotX = 1;
        mModel.pivotY = 0;
    } else if (mMoveFrame > 0 && mLastDirection == 1) {
        // 右移動中：左側に噴射炎
        //   pivot=3 → mX が '^' の位置、左に ".<<" が続く
        mModel.cells  = { ".<<^>" };
        mModel.pivotX = 3;
        mModel.pivotY = 0;
    } else {
        // 通常状態
        mModel.cells  = { "<^>" };
        mModel.pivotX = 1;
        mModel.pivotY = 0;
    }
}

void Player::loseLife_() {
    if (mLives > 0) --mLives;
}

void Player::reset_() {
    mX         = Config::FIELD_WIDTH / 2;
    mY         = Config::FIELD_HEIGHT - 2;
    mLives     = Config::PLAYER_LIVES;
    mHasBullet = false;
    mShootFrame = 0;
    mMoveFrame = 0;
    mLastDirection = 0;
}

bool Player::isActive() const {
    return isAlive_();
}

}  // namespace game
