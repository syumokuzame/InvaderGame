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

    bool isRunning() const { return running_; }

    SceneType getNextSceneType() const { return nextSceneType_; }

    bool hasNextScene() const { return hasNextScene_; }

    void clearNextScene() {
        hasNextScene_ = false;
        nextSceneType_ = SceneType::Title;
    }

protected:
    void quit() { running_ = false; }

    void changeScene(SceneType type) {
        hasNextScene_ = true;
        nextSceneType_ = type;
    }

    Allocator& allocator_;

private:
    bool      running_       = true;
    bool      hasNextScene_  = false;
    SceneType nextSceneType_ = SceneType::Title;
};

}  // namespace engine
