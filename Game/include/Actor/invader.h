#pragma once

#include "Actor/actor.h"

namespace game {

class Invader : public Actor {
public:
    Invader(int x, int y, int row);

    void calc() override;   // 消滅アニメを1フレーム進める
    void draw(engine::Renderer& renderer) const override;
    bool isActive() const override;

    bool isAlive_()     const { return mAlive; }
    bool isSpawning_()  const { return mSpawnFrame < SPAWN_FRAMES; }  // スポーン中？
    bool isFullyDead_() const { return mSpawnFrame >= SPAWN_FRAMES && !mAlive && mDeathTimer <= 0; }
    int  scoreValue_()  const;  // 行によってスコアが異なる

    void kill_();
    void setPos_(int x, int y);
    void spawn_();  // スポーンアニメーション開始

private:
    // 消滅アニメーション: * → + → . → 消滅（各4フレーム計12フレーム）
    static constexpr int DEATH_FRAMES = 12;
    // スポーンアニメーション: . → o → O → * (各2フレーム計8フレーム)
    static constexpr int SPAWN_FRAMES = 8;

    int  mRow;        // 0=上段, 1=中段, 2=下段
    bool mAlive;
    int  mDeathTimer; // 消滅アニメ残りフレーム数 (0=終了)
    int  mSpawnFrame; // スポーンカウンター (0-3=スポーン中, 4以上=スポーン完了)
};

}  // namespace game
