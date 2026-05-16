#pragma once

namespace game {
    class Renderer;
}

namespace engine {

/**
 * @class ActorBase
 * @brief Game層のアクター基底クラス
 * 毎フレーム更新・描画が行われるゲームオブジェクトの基底クラス
 */
class ActorBase {
public:
    ActorBase(int x, int y);
    virtual ~ActorBase() = default;

    // 毎フレーム呼ばれる処理
    virtual void calc() = 0;
    virtual void draw(game::Renderer& renderer) const = 0;

    // 位置情報取得
    int x() const { return x_; }
    int y() const { return y_; }

    // 状態判定
    virtual bool isActive() const = 0;

protected:
    int x_;
    int y_;
};

}  // namespace engine

