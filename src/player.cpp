#include "player.h"
#include "renderer.h"
#include "config.h"

Player::Player()
    : x_(Config::FIELD_WIDTH / 2),
      y_(Config::FIELD_HEIGHT - 2),
      lives_(Config::PLAYER_LIVES),
      hasBullet_(false) {}

void Player::moveLeft() {
    if (x_ > 2) --x_;
}

void Player::moveRight() {
    if (x_ < Config::FIELD_WIDTH - 3) ++x_;
}

bool Player::shoot() {
    // TODO: 弾の実装
    return false;
}

void Player::update() {
    // TODO: 弾の更新
}

// 自機を3文字で描画: <^>
void Player::render(Renderer& renderer) const {
    renderer.draw(x_ - 1, y_, '<');
    renderer.draw(x_,     y_, '^');
    renderer.draw(x_ + 1, y_, '>');
}

void Player::loseLife() {
    if (lives_ > 0) --lives_;
}

void Player::reset() {
    x_         = Config::FIELD_WIDTH / 2;
    y_         = Config::FIELD_HEIGHT - 2;
    lives_     = Config::PLAYER_LIVES;
    hasBullet_ = false;
}
