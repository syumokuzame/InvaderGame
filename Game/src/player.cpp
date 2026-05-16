#include "player.h"
#include "renderer.h"
#include "config.h"

namespace game {

Player::Player()
    : Actor(Config::FIELD_WIDTH / 2, Config::FIELD_HEIGHT - 2),
      lives_(Config::PLAYER_LIVES),
      hasBullet_(false),
      shoot_frame_(0),
      move_frame_(0),
      last_direction_(0) {}

void Player::moveLeft() {
    if (x_ > 2) {
        --x_;
        if (last_direction_ != -1) {
            // 移動方向が左に変わったのでアニメーションをリセット
            move_frame_ = 15;
            last_direction_ = -1;
        } else if (move_frame_ == 0) {
            // 移動継続中で、アニメーションが終わっていればリセット
            move_frame_ = 15;
        }
    }
}

void Player::moveRight() {
    if (x_ < Config::FIELD_WIDTH - 3) {
        ++x_;
        if (last_direction_ != 1) {
            // 移動方向が右に変わったのでアニメーションをリセット
            move_frame_ = 15;
            last_direction_ = 1;
        } else if (move_frame_ == 0) {
            // 移動継続中で、アニメーションが終わっていればリセット
            move_frame_ = 15;
        }
    }
}

bool Player::shoot() {
    if (hasBullet_) return false;  // すでに弾が飛んでいる
    hasBullet_ = true;
    shoot_frame_ = 10;  // 射撃アニメーション開始
    return true;
}

void Player::clearBullet() {
    hasBullet_ = false;
}

void Player::calc() {
    // 射撃アニメーションカウンターをデクリメント
    if (shoot_frame_ > 0) {
        --shoot_frame_;
    }
    
    // 移動アニメーションカウンターをデクリメント
    if (move_frame_ > 0) {
        --move_frame_;
    }
}

// 自機を3文字で描画
void Player::draw(Renderer& renderer) const {
    // 射撃アニメーション中は中央が '|' に変わる
    if (shoot_frame_ > 0) {
        renderer.draw(x_ - 1, y_, '<');
        renderer.draw(x_,     y_, '|');
        renderer.draw(x_ + 1, y_, '>');
    }
    // 移動アニメーション中は傾いた表現
    else if (move_frame_ > 0) {
        if (last_direction_ == -1) {
            // 左移動時：左に傾く表現
            renderer.draw(x_ - 1, y_, '<');
            renderer.draw(x_,     y_, '^');
            renderer.draw(x_ + 1, y_, '>');
            renderer.draw(x_ + 2, y_, '>');
            renderer.draw(x_ + 3, y_, '.');
        } else if (last_direction_ == 1) {
            // 右移動時：右に傾く表現
            renderer.draw(x_ - 3, y_, '.');
            renderer.draw(x_ - 2, y_, '<');
            renderer.draw(x_ - 1, y_, '<');
            renderer.draw(x_,     y_, '^');
            renderer.draw(x_ + 1, y_, '>');
        }
    } else {
        // 通常状態
        renderer.draw(x_ - 1, y_, '<');
        renderer.draw(x_,     y_, '^');
        renderer.draw(x_ + 1, y_, '>');
    }
}

void Player::loseLife() {
    if (lives_ > 0) --lives_;
}

void Player::reset() {
    x_         = Config::FIELD_WIDTH / 2;
    y_         = Config::FIELD_HEIGHT - 2;
    lives_     = Config::PLAYER_LIVES;
    hasBullet_ = false;
    shoot_frame_ = 0;
    move_frame_ = 0;
    last_direction_ = 0;
}

bool Player::isActive() const {
    return isAlive();
}

}  // namespace game
