#pragma once

#include <vector>

namespace engine {

class Renderer;
class ActorBase;

// シーン種別定義（Game層で sceneType を指定するために使用）
enum class SceneType {
    Title,
    Game
};

class SceneBase {
public:
    // Actor管理用メモリを参照として受け取る
    explicit SceneBase(std::vector<ActorBase*>& actors);
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

    // actors の calc() を全て呼び出す
    void updateActors();

    // actors の draw() を全て呼び出す
    void drawActors(Renderer& renderer);

    // actors_ をクリア（各actor をdeleteして解放）
    void clearActors();

protected:
    void quit() { running_ = false; }

    // 派生クラスがシーン切り替えをリクエストする
    void changeScene(SceneType type) {
        hasNextScene_ = true;
        nextSceneType_ = type;
    }

    std::vector<ActorBase*>& actors_;

private:
    bool      running_       = true;
    bool      hasNextScene_  = false;
    SceneType nextSceneType_ = SceneType::Title;
};

}  // namespace engine
