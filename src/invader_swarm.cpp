#include "invader_swarm.h"
#include "bullet.h"
#include "renderer.h"
#include <algorithm>

InvaderSwarm::InvaderSwarm()
    : dx_(1), speed_(20), timer_(0), shootInterval_(60), shootTimer_(0) {
    reset(1);
}

void InvaderSwarm::reset(int level) {
    (void)level;
    invaders_.clear();

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
            invaders_.emplace_back(x, y, row);
            // 敵を即座にスポーン状態にする
            invaders_.back().spawn();
        }
    }
}

void InvaderSwarm::update(std::vector<Bullet>& bullets) {
    // 消滅アニメを進める
    for (auto& inv : invaders_) {
        inv.update();
    }

    // プレイヤー弾との当たり判定
    for (auto& b : bullets) {
        if (b.owner() != BulletOwner::Player || !b.isActive()) continue;
        for (auto& inv : invaders_) {
            if (!inv.isAlive()) continue;
            // 敵機は4マス: (x-1,y), (x,y), (x+1,y), (x,y+1)
            if ((b.x() == inv.x() - 1 && b.y() == inv.y()) ||
                (b.x() == inv.x() && b.y() == inv.y()) ||
                (b.x() == inv.x() + 1 && b.y() == inv.y()) ||
                (b.x() == inv.x() && b.y() == inv.y() + 1)) {
                inv.kill();
                b.deactivate();
                break;
            }
        }
    }

    // 消滅アニメが終わったインベーダーを除去
    invaders_.erase(
        std::remove_if(invaders_.begin(), invaders_.end(),
            [](const Invader& inv) { return inv.isFullyDead(); }),
        invaders_.end()
    );

    // 敵機の移動
    if (invaders_.empty()) return;

    // 生存している敵機の左右の端を計算
    int minX = Config::FIELD_WIDTH, maxX = 0;
    for (const auto& inv : invaders_) {
        if (!inv.isAlive()) continue;
        minX = (inv.x() - 1 < minX) ? (inv.x() - 1) : minX;  // 左端
        maxX = (inv.x() + 1 > maxX) ? (inv.x() + 1) : maxX;  // 右端
    }

    // フィールドの枠内での有効な移動範囲
    const int LEFT_BOUND = 1;              // 左壁の右側
    const int RIGHT_BOUND = Config::FIELD_WIDTH - 2;  // 右壁の左側

    // タイマーをカウント
    ++timer_;
    if (timer_ >= speed_) {
        timer_ = 0;

        // 方向変更チェック（枠に到達したか）
        if ((dx_ > 0 && maxX >= RIGHT_BOUND) ||
            (dx_ < 0 && minX <= LEFT_BOUND)) {
            dx_ = -dx_;  // 方向を反転
        }

        // 全敵機を移動
        for (auto& inv : invaders_) {
            if (!inv.isAlive()) continue;
            inv.setPos(inv.x() + dx_, inv.y());
        }
    }
}

void InvaderSwarm::render(Renderer& renderer) const {
    for (const auto& inv : invaders_) {
        inv.render(renderer);
    }
}

bool InvaderSwarm::allDefeated() const {
    for (const auto& inv : invaders_) {
        if (inv.isAlive()) return false;
    }
    return invaders_.empty();
}

bool InvaderSwarm::hasReachedBottom() const {
    for (const auto& inv : invaders_) {
        // 敵は y と y+1 の2行占めるため、y+1がプレイヤーレベル以上なら判定
        if (inv.isAlive() && inv.y() + 1 >= Config::FIELD_HEIGHT - 2)
            return true;
    }
    return false;
}
