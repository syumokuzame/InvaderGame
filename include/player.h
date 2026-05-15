#pragma once

#include "config.h"

class Bullet;
class Renderer;

class Player {
public:
    Player();

    void moveLeft();
    void moveRight();
    bool shoot();        // 発射成功なら true

    void update();
    void render(Renderer& renderer) const;

    int  x()      const { return x_; }
    int  y()      const { return y_; }
    int  lives()  const { return lives_; }
    bool isAlive() const { return lives_ > 0; }

    void loseLife();
    void reset();
    void clearBullet();   // 弾が画面から消えたときに呼ぶ

private:
    int  x_;
    int  y_;
    int  lives_;
    bool hasBullet_;  // 弾が画面上に存在するか
};
