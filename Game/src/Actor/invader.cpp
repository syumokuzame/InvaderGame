#include "Actor/invader.h"
#include "Renderer.h"

namespace game {

// 行ごとの生存時キャラクター（上段=M, 中段=W, 下段=V）
static constexpr char ROW_CHARS[] = { 'M', 'W', 'V' };

Invader::Invader(int x, int y, int row)
    : Actor(x, y), mRow(row), mAlive(false), mDeathTimer(0), mSpawnFrame(0) {}

void Invader::calc() {
    // スポーン中：フレームをインクリメント
    if (mSpawnFrame < SPAWN_FRAMES) {
        mSpawnFrame++;
        if (mSpawnFrame >= SPAWN_FRAMES) {
            mAlive = true;  // スポーン完了→有効化
        }
        return;
    }
    // スポーン完了後：消滅アニメ処理
    if (mDeathTimer > 0) --mDeathTimer;
}

void Invader::kill_() {
    mAlive      = false;
    mDeathTimer = DEATH_FRAMES;
}

void Invader::setPos_(int x, int y) {
    mX = x;
    mY = y;
}

void Invader::spawn_() {
    mSpawnFrame = 0;
    mAlive = false;  // スポーン完了時に true になる
    mDeathTimer = 0;
}

int Invader::scoreValue_() const {
    switch (mRow) {
        case 0: return 30;  // 上段
        case 1: return 20;  // 中段
        default: return 10; // 下段
    }
}

// 敵機のサイズ定数
static constexpr int INVADER_WIDTH = 3;   // 敵の幅: 3 (中心-1 から 中心+1)
static constexpr int INVADER_HEIGHT = 2;  // 敵の高さ: 2 行

void Invader::draw(engine::Renderer& renderer) const {
    // スポーン中：アニメーション表現
    if (mSpawnFrame < SPAWN_FRAMES) {
        char spawnChars[] = { '.', 'o', 'O', '*' };
        char ch = spawnChars[mSpawnFrame / 2];
        renderer.draw(mX - 1, mY, ch);
        renderer.draw(mX, mY, ch);
        renderer.draw(mX + 1, mY, ch);
        renderer.draw(mX, mY + 1, ch);
        return;
    }
    // スポーン完了後：敵機表示
    if (mAlive) {
        // 敵機をTミノ形で描画（上向き）
        // 上行: 左-中-右
        renderer.draw(mX - 1, mY, '*');
        renderer.draw(mX, mY, '*');
        renderer.draw(mX + 1, mY, '*');
        // 下行: 中心
        renderer.draw(mX, mY + 1, '*');
        return;
    }
    // 消滅アニメーション（3段階: * → + → .）
    if (mDeathTimer > DEATH_FRAMES * 2 / 3) {
        renderer.draw(mX - 1, mY, '*');
        renderer.draw(mX, mY, '*');
        renderer.draw(mX + 1, mY, '*');
        renderer.draw(mX, mY + 1, '*');
    } else if (mDeathTimer > DEATH_FRAMES / 3) {
        renderer.draw(mX - 1, mY, '+');
        renderer.draw(mX, mY, '+');
        renderer.draw(mX + 1, mY, '+');
        renderer.draw(mX, mY + 1, '+');
    } else if (mDeathTimer > 0) {
        renderer.draw(mX - 1, mY, '.');
        renderer.draw(mX, mY, '.');
        renderer.draw(mX + 1, mY, '.');
        renderer.draw(mX, mY + 1, '.');
    }
    // mDeathTimer == 0: 完全消滅（何も描画しない）
}

bool Invader::isActive() const {
    return mSpawnFrame < SPAWN_FRAMES || mAlive;
}

}  // namespace game
