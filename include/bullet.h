#pragma once

class Renderer;

enum class BulletOwner { Player, Enemy };

class Bullet {
public:
    Bullet(int x, int y, BulletOwner owner);

    void update();
    void render(Renderer& renderer) const;

    int  x()       const { return x_; }
    int  y()       const { return y_; }
    bool isActive() const { return active_; }
    BulletOwner owner() const { return owner_; }

    void deactivate();

private:
    int         x_;
    int         y_;
    int         dy_;   // 移動方向（プレイヤー: -1, 敵: +1）
    bool        active_;
    BulletOwner owner_;
};
