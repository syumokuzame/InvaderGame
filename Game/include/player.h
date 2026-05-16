#pragma once

#include "config.h"
#include "actor.h"

class Bullet;
class Renderer;

namespace game {

class Player : public Actor {
public:
    Player();

    void moveLeft();
    void moveRight();
    bool shoot();        // 発射成功なら true

    void calc() override;
    void draw(Renderer& renderer) const override;
    bool isActive() const override;

    int  lives()  const { return lives_; }
    bool isAlive() const { return lives_ > 0; }

    void loseLife();
    void reset();
    void clearBullet();   // 弾が画面から消えたときに呼ぶ

private:
    int  lives_;
    bool hasBullet_;      // 弾が画面上に存在するか
    
    // アニメーション用
    int shoot_frame_;     // 射撃アニメーションカウンター (0=無効, 1-10=射撃中)
    int move_frame_;      // 移動アニメーションカウンター (0=無効, 1-15=移動中)
    int last_direction_;  // 最後の移動方向 (-1=左, 0=停止, 1=右)
};

}  // namespace game
