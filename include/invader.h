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
    bool isFullyDead() const { return !alive_ && deathTimer_ <= 0; }
    int  scoreValue()  const;  // 行によってスコアが異なる

    void kill();
    void setPos(int x, int y);

private:
    // 消滅アニメーション: * → + → . → 消滅（各4フレーム計12フレーム）
    static constexpr int DEATH_FRAMES = 12;

    int  x_;
    int  y_;
    int  row_;        // 0=上段, 1=中段, 2=下段
    bool alive_;
    int  deathTimer_; // 消滅アニメ残りフレーム数 (0=終了)
};
