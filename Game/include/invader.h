#pragma once

#include "actor.h"

namespace game {

class Invader : public Actor {
public:
    Invader(int x, int y, int row);

    void calc() override;   // 消滅アニメを1フレーム進める
    void draw(engine::Renderer& renderer) const override;
    bool isActive() const override;

    bool isAlive()     const { return alive_; }
    bool isSpawning()  const { return spawnFrame_ < SPAWN_FRAMES; }  // スポーン中？
    bool isFullyDead() const { return spawnFrame_ >= SPAWN_FRAMES && !alive_ && deathTimer_ <= 0; }
    int  scoreValue()  const;  // 行によってスコアが異なる

    void kill();
    void setPos(int x, int y);
    void spawn();  // スポーンアニメーション開始

private:
    // 消滅アニメーション: * → + → . → 消滅（各4フレーム計12フレーム）
    static constexpr int DEATH_FRAMES = 12;
    // スポーンアニメーション: . → o → O → * (各2フレーム計8フレーム)
    static constexpr int SPAWN_FRAMES = 8;

    int  row_;        // 0=上段, 1=中段, 2=下段
    bool alive_;
    int  deathTimer_; // 消滅アニメ残りフレーム数 (0=終了)
    int  spawnFrame_; // スポーンカウンター (0-3=スポーン中, 4以上=スポーン完了)
};

}  // namespace game
