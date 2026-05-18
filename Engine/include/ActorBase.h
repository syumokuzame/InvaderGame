#pragma once

#include "ActorModel.h"
#include "ColliderComponent.h"
#include "RenderQueue.h"

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

    // Phase 1: 移動・アニメーション更新（全アクター必須）
    virtual void preCalc() = 0;

    // Phase 2: 移動後の状態を使った当たり判定（必要なアクターのみオーバーライド）
    virtual void postCalc() {}

    // デフォルト実装: mModel を RenderQueue に submitModel() で登録する
    // calc() 内で mModel を更新することでアニメーションを実現する
    // オーバーライドも可能
    virtual void draw() const;

    int x() const { return mX; }
    int y() const { return mY; }

    virtual bool isActive() const = 0;

    // 当たり判定コンポーネント（当たり判定が不要な Actor はデフォルト nullptr）
    virtual ColliderComponent* collider() { return nullptr; }

protected:
    int mX;
    int mY;

    // 描画モデル（calc() 内で更新する）
    ActorModel mModel;

    // 描画レイヤー（サブクラスでオーバーライド可能）
    virtual int drawLayer() const { return static_cast<int>(RenderLayer::Actor); }
};

}  // namespace engine

