#pragma once

#include "config.h"
#include "Actor/actor.h"

class Bullet;

namespace game {

class Player : public Actor {
public:
    Player();

    void moveLeft_();
    void moveRight_();
    bool shoot_();        // 発射成功なら true

    void calc() override;
    bool isActive() const override;

    int  lives_()  const { return mLives; }
    bool isAlive_() const { return mLives > 0; }

    void loseLife_();
    void reset_();
    void clearBullet_();   // 弾が画面から消えたときに呼ぶ

private:
    int  mLives;
    bool mHasBullet;      // 弾が画面上に存在するか

    int mShootFrame;     // 射撃アニメーションカウンター (0=無効, 1-10=射撃中)
    int mMoveFrame;      // 移動アニメーションカウンター (0=無効, 1-15=移動中)
    int mLastDirection;  // 最後の移動方向 (-1=左, 0=停止, 1=右)
};

}  // namespace game
