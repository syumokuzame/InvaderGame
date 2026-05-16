#pragma once

namespace engine {

class Renderer;

// シーン種別定義（Game層で sceneType を指定するために使用）
enum class SceneType {
    Title,
    Game
};

class SceneBase {
public:
    virtual ~SceneBase() = default;
    virtual void calc() = 0;
    virtual void draw(Renderer& renderer) = 0;

    bool isRunning() const { return running_; }

    // FrameWork が呼び出す：次シーンの種別を取得（0 なら切り替えなし）
    SceneType getNextSceneType() const {
        return nextSceneType_;
    }

    bool hasNextScene() const {
        // SceneType は enum class で、デフォルト値がないため、フラグで判定
        return hasNextScene_;
    }

    // デバッグ用：次シーン情報をクリア（Framework がシーン切り替え後に呼ぶ）
    void clearNextScene() {
        hasNextScene_ = false;
        nextSceneType_ = SceneType::Title;
    }

protected:
    void quit() { running_ = false; }

    // 派生クラスがシーン切り替えをリクエストする
    void changeScene(SceneType type) {
        hasNextScene_ = true;
        nextSceneType_ = type;
    }

private:
    bool      running_       = true;
    bool      hasNextScene_  = false;
    SceneType nextSceneType_ = SceneType::Title;
};

}  // namespace engine
