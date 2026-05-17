#pragma once

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

    virtual void calc() = 0;
    // RenderQueue::instance() に描画コマンドを登録する
    virtual void draw() const = 0;

    int x() const { return mX; }
    int y() const { return mY; }

    virtual bool isActive() const = 0;

protected:
    int mX;
    int mY;
};

}  // namespace engine

