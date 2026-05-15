#include "invader.h"
#include "renderer.h"

// 行ごとの生存時キャラクター（上段=M, 中段=W, 下段=V）
static constexpr char ROW_CHARS[] = { 'M', 'W', 'V' };

Invader::Invader(int x, int y, int row)
    : x_(x), y_(y), row_(row), alive_(true), deathTimer_(0) {}

void Invader::update() {
    if (deathTimer_ > 0) --deathTimer_;
}

void Invader::kill() {
    alive_      = false;
    deathTimer_ = DEATH_FRAMES;
}

void Invader::setPos(int x, int y) {
    x_ = x;
    y_ = y;
}

int Invader::scoreValue() const {
    switch (row_) {
        case 0: return 30;  // 上段
        case 1: return 20;  // 中段
        default: return 10; // 下段
    }
}

// 敵機のサイズ定数
static constexpr int INVADER_WIDTH = 3;   // 敵の幅: 3 (中心-1 から 中心+1)
static constexpr int INVADER_HEIGHT = 2;  // 敵の高さ: 2 行

void Invader::render(Renderer& renderer) const {
    if (alive_) {
        // 敵機をTミノ形で描画（上向き）
        // 上行: 左-中-右
        renderer.draw(x_ - 1, y_, '*');
        renderer.draw(x_, y_, '*');
        renderer.draw(x_ + 1, y_, '*');
        // 下行: 中心
        renderer.draw(x_, y_ + 1, '*');
        return;
    }
    // 消滅アニメーション（3段階: * → + → .）
    if (deathTimer_ > DEATH_FRAMES * 2 / 3) {
        renderer.draw(x_ - 1, y_, '*');
        renderer.draw(x_, y_, '*');
        renderer.draw(x_ + 1, y_, '*');
        renderer.draw(x_, y_ + 1, '*');
    } else if (deathTimer_ > DEATH_FRAMES / 3) {
        renderer.draw(x_ - 1, y_, '+');
        renderer.draw(x_, y_, '+');
        renderer.draw(x_ + 1, y_, '+');
        renderer.draw(x_, y_ + 1, '+');
    } else if (deathTimer_ > 0) {
        renderer.draw(x_ - 1, y_, '.');
        renderer.draw(x_, y_, '.');
        renderer.draw(x_ + 1, y_, '.');
        renderer.draw(x_, y_ + 1, '.');
    }
    // deathTimer_ == 0: 完全消滅（何も描画しない）
}
