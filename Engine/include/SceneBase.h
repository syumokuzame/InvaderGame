#pragma once

#include "Allocator.h"

namespace engine {

class Renderer;

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
    virtual void draw(Renderer& renderer) = 0;

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

    Allocator& mAllocator;

private:
    bool      mRunning       = true;
    bool      mHasNextScene  = false;
    SceneType mNextSceneType = SceneType::Title;
};

}  // namespace engine
