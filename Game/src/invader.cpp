#include "invader.h"
#include "renderer.h"

namespace game {

// 行ごとの生存時キャラクター（上段=M, 中段=W, 下段=V）
static constexpr char ROW_CHARS[] = { 'M', 'W', 'V' };

Invader::Invader(int x, int y, int row)
    : Actor(x, y), row_(row), alive_(false), deathTimer_(0), spawnFrame_(0) {}

void Invader::calc() {
    // スポーン中：フレームをインクリメント
    if (spawnFrame_ < SPAWN_FRAMES) {
        spawnFrame_++;
        if (spawnFrame_ >= SPAWN_FRAMES) {
            alive_ = true;  // スポーン完了→有効化
        }
        return;
    }
    // スポーン完了後：消滅アニメ処理
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

void Invader::spawn() {
    spawnFrame_ = 0;
    alive_ = false;  // スポーン完了時に true になる
    deathTimer_ = 0;
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

void Invader::draw(Renderer& renderer) const {
    // スポーン中：アニメーション表現
    if (spawnFrame_ < SPAWN_FRAMES) {
        char spawnChars[] = { '.', 'o', 'O', '*' };
        char ch = spawnChars[spawnFrame_ / 2];
        renderer.draw(x_ - 1, y_, ch);
        renderer.draw(x_, y_, ch);
        renderer.draw(x_ + 1, y_, ch);
        renderer.draw(x_, y_ + 1, ch);
        return;
    }
    // スポーン完了後：敵機表示
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

bool Invader::isActive() const {
    return spawnFrame_ < SPAWN_FRAMES || alive_;
}

}  // namespace game
