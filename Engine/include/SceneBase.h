#pragma once

#include "ActorBase.h"
#include "Allocator.h"
#include "JobQueue.h"
#include "UIBase.h"
#include <memory>
#include <vector>

namespace engine {

// シーン種別定義（Game層で sceneType を指定するために使用）
enum class SceneType {
    Title,
    Game
};

class SceneBase {
public:
    // 汎用アロケーターを参照として受け取る
    explicit SceneBase(Allocator& allocator);
    virtual ~SceneBase() = default;

    // フロー全体を管理（non-virtual）
    // sceneCalcImpl_() → JobQueue(preCalc→collision→postCalc→UI) → cleanup
    void calc();

    // RenderQueue::instance() に描画コマンドを登録する
    virtual void draw() = 0;

    bool isRunning() const { return mRunning; }

    SceneType getNextSceneType() const { return mNextSceneType; }

    bool hasNextScene() const { return mHasNextScene; }

    void clearNextScene() {
        mHasNextScene = false;
        mNextSceneType = SceneType::Title;
    }

protected:
    // Game層がoverride: シーンのビジネスロジックを記述する。
    // ここでは直接 Actor の calc を呼ばない（JobQueue 経由で自動実行される）。
    // 前フレームの postCalc 後の状態を参照してスコア計算・状態遷移等を行う。
    virtual void sceneCalcImpl_() = 0;

    void quit() { mRunning = false; }

    void changeScene(SceneType type) {
        mHasNextScene = true;
        mNextSceneType = type;
    }

    // UIを追加し生ポインタを返す（所有権はmUIs_が持つ）
    // priority: calc の実行優先度（値が小さいほど先に実行。デフォルト 0）
    UIBase* addUI_(std::unique_ptr<UIBase> ui, int priority = 0);

    // 全UIの draw() を一括呼び出し（draw() から呼ぶ）
    void drawUIs_();

    // アクター管理
    // priority: preCalc / postCalc の実行優先度（値が小さいほど先に実行。デフォルト 0）
    void registerActor_(ActorBase* actor, int priority = 0);

    // 登録中の全アクターの draw() を呼ぶ
    void drawActors_() const;

    // 背景（枠線）を描画する
    void drawBackground_(int width, int height);

    // --- 衝突判定専用リスト ---
    // mActors_ には入らず、衝突走査のみに参加する Actor を登録する
    // （Invader など、ライフサイクル管理は別で行うオブジェクト向け）
    // priority: 現時点では衝突走査の順序には影響しない（将来の拡張用）
    void registerCollider_(ActorBase* actor, int priority = 0);

    // mColliders_ を全消去する（リセット前に呼ぶ）
    void clearColliders_();

    Allocator& mAllocator;

private:
    // preCalc〜postCalc 間に走らせる衝突走査
    // mActors_ と mColliders_ の collider() 持ちを総当たりで判定する
    void calcCollisions_();

    // isActive()==false のエントリを除去（SceneBase::calc() 末尾で自動呼び出し）
    void cleanupActors_();
    void cleanupColliders_();

    // --- Actor / Collider / UI エントリ（優先度付き） ---
    struct ActorEntry { ActorBase* actor; int priority = 0; };
    struct UIEntry    { std::unique_ptr<UIBase> ui; int priority = 0; };

    bool      mRunning       = true;
    bool      mHasNextScene  = false;
    SceneType mNextSceneType = SceneType::Title;

    std::vector<UIEntry>    mUIs_;
    std::vector<ActorEntry> mActors_;
    std::vector<ActorEntry> mColliders_;  // 衝突走査のみ参加するアクター

    JobQueue mJobQueue_;
};

}  // namespace engine
