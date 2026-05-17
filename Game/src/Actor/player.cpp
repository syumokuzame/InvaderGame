#include "Actor/player.h"
#include "RenderQueue.h"
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
}

// 自機を3文字で描画
void Player::draw() const {
    auto& rq = engine::RenderQueue::instance();
    // 射撃アニメーション中は中央が '|' に変わる
    if (mShootFrame > 0) {
        rq.submit(mX - 1, mY, '<');
        rq.submit(mX,     mY, '|');
        rq.submit(mX + 1, mY, '>');
    }
    // 移動アニメーション中は傾いた表現
    else if (mMoveFrame > 0) {
        if (mLastDirection == -1) {
            // 左移動時：左に傾く表現
            rq.submit(mX - 1, mY, '<');
            rq.submit(mX,     mY, '^');
            rq.submit(mX + 1, mY, '>');
            rq.submit(mX + 2, mY, '>');
            rq.submit(mX + 3, mY, '.');
        } else if (mLastDirection == 1) {
            // 右移動時：右に傾く表現
            rq.submit(mX - 3, mY, '.');
            rq.submit(mX - 2, mY, '<');
            rq.submit(mX - 1, mY, '<');
            rq.submit(mX,     mY, '^');
            rq.submit(mX + 1, mY, '>');
        }
    } else {
        // 通常状態
        rq.submit(mX - 1, mY, '<');
        rq.submit(mX,     mY, '^');
        rq.submit(mX + 1, mY, '>');
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
