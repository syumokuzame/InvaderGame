#pragma once

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

    Allocator& mAllocator;

private:
    bool      mRunning       = true;
    bool      mHasNextScene  = false;
    SceneType mNextSceneType = SceneType::Title;

    std::vector<std::unique_ptr<UIBase>> mUIs_;
};

}  // namespace engine
