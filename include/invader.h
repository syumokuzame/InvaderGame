#pragma once

class Renderer;

class Invader {
public:
    Invader(int x, int y, int row);

    void render(Renderer& renderer) const;

    int  x()        const { return x_; }
    int  y()        const { return y_; }
    bool isAlive()  const { return alive_; }
    int  scoreValue() const;  // 行によってスコアが異なる

    void kill();
    void setPos(int x, int y);

private:
    int  x_;
    int  y_;
    int  row_;    // 0=上段, 1=中段, 2=下段
    bool alive_;
};
