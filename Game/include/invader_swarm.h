#pragma once

#include "invader.h"
#include "config.h"
#include <vector>

class Renderer;

namespace game {

class Bullet;

class InvaderSwarm {
public:
    InvaderSwarm();

    void reset(int level);    // レベルに応じて速度・頻度を設定
    void update(std::vector<Bullet>& bullets);
    void render(Renderer& renderer) const;

    bool allDefeated() const;
    bool hasReachedBottom() const;

    std::vector<Invader>& invaders() { return invaders_; }

private:
    std::vector<Invader> invaders_;
    int  dx_;          // 横移動方向 (+1 or -1)
    int  speed_;       // 移動間隔（フレーム数）
    int  timer_;
    int  shootInterval_;
    int  shootTimer_;

    void moveDown();
};

}  // namespace game
