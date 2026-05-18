#include "Actor/invader_swarm.h"
#include "Actor/bullet.h"
#include "RenderQueue.h"
#include <algorithm>
#include <list>

namespace game {

InvaderSwarm::InvaderSwarm(std::list<Bullet>& bullets)
    : engine::ActorBase(0, 0),
      mBullets_(bullets),
      mDx(1), mSpeed(20), mTimer(0), mShootInterval(60), mShootTimer(0) {
    reset_(1);
}

void InvaderSwarm::reset_(int level) {
    (void)level;
    mInvaders.clear();

    // 敵機のサイズ定数
    constexpr int INVADER_WIDTH = 3;   // 敵の幅 (中心 ± 1)
    constexpr int INVADER_HEIGHT = 2;  // 敵の高さ (2行分)
    constexpr int SPACING_X = 1;       // 敵同士の間隔（横）
    constexpr int SPACING_Y = 1;       // 敵同士の間隔（縦）

    // フィールドサイズに応じて敵機を動的配置
    const int MARGIN_X = 2;            // 左右マージン: 各2文字
    const int MARGIN_Y = Config::UI_HEIGHT + 2;  // 上マージン（UI領域の下 + 2行分）
    const int MARGIN_BOTTOM = 3;       // 下マージン（プレイヤーとの距離確保）

    // 有効な配置幅と高さ
    const int available_width = Config::FIELD_WIDTH - MARGIN_X * 2;
    const int available_height = Config::FIELD_HEIGHT - MARGIN_Y - MARGIN_BOTTOM;

    // 敵1体分の必要スペース（敵本体+間隔）
    const int slot_width = INVADER_WIDTH + SPACING_X;
    const int slot_height = INVADER_HEIGHT + SPACING_Y;

    // 敵の列数と行数を計算
    const int cols = (available_width + SPACING_X) / slot_width;
    const int rows = (available_height + SPACING_Y) / slot_height;

    // 最大3行, 最低2行, 最大10列程度に制限
    int actual_rows = rows > 3 ? 3 : (rows < 2 ? 2 : rows);
    int actual_cols = cols > 10 ? 10 : (cols < 3 ? 3 : cols);

    // 計算した敵数に応じて動的にフィールド内配置
    const int total_width = actual_cols * slot_width - SPACING_X;

    // 敵群をフィールド中央に配置
    const int start_x = MARGIN_X + (available_width - total_width) / 2 + 1;  // 敵の中心座標
    const int start_y = MARGIN_Y;

    for (int row = 0; row < actual_rows; ++row) {
        for (int col = 0; col < actual_cols; ++col) {
            int x = start_x + col * slot_width;
            int y = start_y + row * slot_height;
            mInvaders.emplace_back(x, y, row);
            // 敵を即座にスポーン状態にする
            mInvaders.back().spawn_();
        }
    }
}

void InvaderSwarm::preCalc() {
    // 各インベーダーのアニメーション更新
    for (auto& inv : mInvaders) {
        if (inv.isActive()) inv.preCalc();
    }

    // 消滅アニメが終わったインベーダーを除去
    mInvaders.erase(
        std::remove_if(mInvaders.begin(), mInvaders.end(),
            [](const Invader& inv) { return inv.isFullyDead_(); }),
        mInvaders.end()
    );

    // 敵機の移動
    if (mInvaders.empty()) return;

    // 生存している敵機の左右の端を計算
    int minX = Config::FIELD_WIDTH, maxX = 0;
    for (const auto& inv : mInvaders) {
        if (!inv.isAlive_()) continue;
        minX = (inv.x() - 1 < minX) ? (inv.x() - 1) : minX;  // 左端
        maxX = (inv.x() + 1 > maxX) ? (inv.x() + 1) : maxX;  // 右端
    }

    // フィールドの枠内での有効な移動範囲
    const int LEFT_BOUND = 1;
    const int RIGHT_BOUND = Config::FIELD_WIDTH - 2;

    ++mTimer;
    if (mTimer >= mSpeed) {
        mTimer = 0;

        if ((mDx > 0 && maxX >= RIGHT_BOUND) ||
            (mDx < 0 && minX <= LEFT_BOUND)) {
            mDx = -mDx;
        }

        for (auto& inv : mInvaders) {
            if (!inv.isAlive_()) continue;
            inv.setPos_(inv.x() + mDx, inv.y());
        }
    }
}

void InvaderSwarm::postCalc() {
    // プレイヤー弾との当たり判定（全アクターの移動が確定した後）
    for (auto& b : mBullets_) {
        if (b.owner() != BulletOwner::Player || !b.isActive()) continue;
        for (auto& inv : mInvaders) {
            if (!inv.isAlive_()) continue;
            // 敵機は4マス: (x-1,y), (x,y), (x+1,y), (x,y+1)
            if ((b.x() == inv.x() - 1 && b.y() == inv.y()) ||
                (b.x() == inv.x()     && b.y() == inv.y()) ||
                (b.x() == inv.x() + 1 && b.y() == inv.y()) ||
                (b.x() == inv.x()     && b.y() == inv.y() + 1)) {
                inv.kill_();
                b.deactivate_();
                break;
            }
        }
    }
}

void InvaderSwarm::draw() const {
    for (const auto& inv : mInvaders) {
        inv.draw();
    }
}

bool InvaderSwarm::allDefeated_() const {
    for (const auto& inv : mInvaders) {
        if (!inv.isFullyDead_()) return false;  // スポーン中・生存中・消滅アニメ中のどれかがいれば false
    }
    return true;  // mInvaders が空、または全て完全消滅の場合のみ true
}

bool InvaderSwarm::hasReachedBottom_() const {
    for (const auto& inv : mInvaders) {
        // 敵は y と y+1 の2行占めるため、y+1がプレイヤーレベル以上なら判定
        if (inv.isAlive_() && inv.y() + 1 >= Config::FIELD_HEIGHT - 2)
            return true;
    }
    return false;
}

}  // namespace game
