#pragma once

#include <cstdint>
#include <vector>

namespace engine {

class ActorBase;

// 衝突グループ（ビットマスク）― 将来の拡張のため namespace にまとめる
namespace CollisionGroup {
    constexpr uint32_t None         = 0;
    constexpr uint32_t PlayerBullet = 1 << 0;  // プレイヤーの弾
    constexpr uint32_t EnemyBody    = 1 << 1;  // 敵本体
    // 将来拡張: EnemyBullet = 1 << 2, PlayerBody = 1 << 3, ...
}

// 衝突結果1件（postCalc でコンポーネントから取得する）
struct HitResult {
    ActorBase* other;   // 衝突相手の Actor
    int x, y;           // 衝突が発生したワールド座標
};

// 当たり判定コンポーネント
// Actor が保持し、SceneBase の collision フェーズで自動的に書き込まれる
class ColliderComponent {
public:
    ColliderComponent(uint32_t group, uint32_t mask);

    // --- ヒットボックス定義 ---
    // Actor 座標 (mX, mY) からの相対オフセットでセルを追加する
    void addCell(int dx, int dy);

    uint32_t group() const { return mGroup; }
    uint32_t mask()  const { return mMask; }

    // --- 衝突結果（SceneBase から書き込まれる） ---
    void clearHits();
    void addHit(ActorBase* other, int x, int y);

    const std::vector<HitResult>& hits() const { return mHits; }
    bool hasHit() const { return !mHits.empty(); }

    // ワールド座標のセルリストを返す（衝突走査で使用）
    struct Cell { int x, y; };
    std::vector<Cell> worldCells(int originX, int originY) const;

private:
    uint32_t mGroup;
    uint32_t mMask;
    std::vector<std::pair<int, int>> mCells;  // 相対オフセット
    std::vector<HitResult>           mHits;
};

}  // namespace engine
