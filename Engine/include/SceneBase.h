#pragma once

#include "ActorBase.h"
#include "Allocator.h"
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
    virtual void calc() = 0;
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
    void quit() { mRunning = false; }

    void changeScene(SceneType type) {
        mHasNextScene = true;
        mNextSceneType = type;
    }

    // UIを追加し生ポインタを返す（所有権はmUIs_が持つ）
    // 返り値でGameScene等が特定UIへの参照を保持できる
    UIBase* addUI_(std::unique_ptr<UIBase> ui);

    // 全UIのcalc/drawを一括処理（SceneBaseサブクラスのcalc/drawから呼ぶ）
    void calcUIs_();
    void drawUIs_();

    // アクター管理
    // 登録順に mActors_ へ追加する（所有権は呼び出し元が保持）
    void registerActor_(ActorBase* actor);

    // Phase1(preCalc全員) → Phase2(postCalc全員) の2フェーズで走査する
    void calcActors_();

    // isActive()==false のアクターを mActors_ から除去する
    // ストレージ側の解放より先に呼ぶこと
    void cleanupActors_();

    // 登録中の全アクターの draw() を呼ぶ
    void drawActors_() const;

    // 背景（枠線）を描画する
    void drawBackground_(int width, int height);

    // --- 衝突判定専用リスト ---
    // mActors_ には入らず、衝突走査のみに参加する Actor を登録する
    // （Invader など、ライフサイクル管理は別で行うオブジェクト向け）
    void registerCollider_(ActorBase* actor);

    // !isActive() のエントリを mColliders_ から除去する
    // cleanupActors_() の後、ストレージ側の erase より前に呼ぶこと
    void cleanupColliders_();

    // mColliders_ を全消去する（リセット前に呼ぶ）
    void clearColliders_();

    Allocator& mAllocator;

private:
    // preCalc〜postCalc 間に走らせる衝突走査
    // mActors_ と mColliders_ の collider() 持ちを総当たりで判定する
    void calcCollisions_();

    bool      mRunning       = true;
    bool      mHasNextScene  = false;
    SceneType mNextSceneType = SceneType::Title;

    std::vector<std::unique_ptr<UIBase>> mUIs_;
    std::vector<ActorBase*>              mActors_;
    std::vector<ActorBase*>              mColliders_;  // 衝突走査のみ参加するアクター
};

}  // namespace engine
