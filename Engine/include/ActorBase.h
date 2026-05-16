#pragma once

namespace engine {

class Renderer;

/**
 * @class ActorBase
 * @brief Game層のアクター基底クラス
 * 毎フレーム更新・描画が行われるゲームオブジェクトの基底クラス
 */
class ActorBase {
public:
    ActorBase(int x, int y);
    virtual ~ActorBase() = default;

    virtual void calc() = 0;
    virtual void draw(Renderer& renderer) const = 0;

    int x() const { return x_; }
    int y() const { return y_; }

    virtual bool isActive() const = 0;

protected:
    int x_;
    int y_;
};

}  // namespace engine

