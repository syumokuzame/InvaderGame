#pragma once

class Renderer;

class Invader {
public:
    Invader(int x, int y, int row);

    void update();   // 消滅アニメを1フレーム進める
    void render(Renderer& renderer) const;

    int  x()           const { return x_; }
    int  y()           const { return y_; }
    bool isAlive()     const { return alive_; }
    bool isSpawning()  const { return spawnFrame_ < SPAWN_FRAMES; }  // スポーン中？
    bool isActive()    const { return spawnFrame_ < SPAWN_FRAMES || alive_; }  // スポーン中または生存中
    bool isFullyDead() const { return spawnFrame_ >= SPAWN_FRAMES && !alive_ && deathTimer_ <= 0; }
    int  scoreValue()  const;  // 行によってスコアが異なる

    void kill();
    void setPos(int x, int y);
    void spawn();  // スポーンアニメーション開始

private:
    // 消滅アニメーション: * → + → . → 消滅（各4フレーム計12フレーム）
    static constexpr int DEATH_FRAMES = 12;
    // スポーンアニメーション: . → o → O → * (各1フレーム計4フレーム)
    static constexpr int SPAWN_FRAMES = 4;

    int  x_;
    int  y_;
    int  row_;        // 0=上段, 1=中段, 2=下段
    bool alive_;
    int  deathTimer_; // 消滅アニメ残りフレーム数 (0=終了)
    int  spawnFrame_; // スポーンカウンター (0-3=スポーン中, 4以上=スポーン完了)
};
