#include "Actor/invader.h"

namespace game {

Invader::Invader(int x, int y, int row)
    : Actor(x, y), mRow(row), mAlive(false), mDeathTimer(0), mSpawnFrame(0) {}

void Invader::calc() {
    // スポーン中：フレームをインクリメント
    if (mSpawnFrame < SPAWN_FRAMES) {
        mSpawnFrame++;
        if (mSpawnFrame >= SPAWN_FRAMES) {
            mAlive = true;  // スポーン完了→有効化
        }
        // スポーンアニメーションのモデル更新
        // pivot=(1,0) → mX が中央
        const char spawnChars[] = { '.', 'o', 'O', '*' };
        char ch = spawnChars[(mSpawnFrame - 1) / 2];
        std::string top(3, ch);     // "***" など
        std::string bot = " ";
        bot += ch;
        bot += " ";                 // " * "
        mModel.cells  = { top, bot };
        mModel.pivotX = 1;
        mModel.pivotY = 0;
        return;
    }

    // スポーン完了後：消滅アニメ処理
    if (mDeathTimer > 0) --mDeathTimer;

    // モデル更新
    if (mAlive) {
        // 生存時：Tミノ形 (pivot=(1,0))
        mModel.cells  = { "***", " * " };
        mModel.pivotX = 1;
        mModel.pivotY = 0;
    } else if (mDeathTimer > DEATH_FRAMES * 2 / 3) {
        mModel.cells  = { "***", " * " };
        mModel.pivotX = 1;
        mModel.pivotY = 0;
    } else if (mDeathTimer > DEATH_FRAMES / 3) {
        mModel.cells  = { "+++", " + " };
        mModel.pivotX = 1;
        mModel.pivotY = 0;
    } else if (mDeathTimer > 0) {
        mModel.cells  = { "...", " . " };
        mModel.pivotX = 1;
        mModel.pivotY = 0;
    } else {
        // 完全消滅（モデルを空に）
        mModel.cells = {};
    }
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

bool Invader::isActive() const {
    return mSpawnFrame < SPAWN_FRAMES || mAlive;
}

}  // namespace game
