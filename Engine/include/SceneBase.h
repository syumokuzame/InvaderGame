#pragma once

namespace engine {

class Renderer;

class SceneBase {
public:
    virtual ~SceneBase() { delete nextScene_; }
    virtual void calc() = 0;
    virtual void draw(Renderer& renderer) = 0;

    bool isRunning() const { return running_; }

    // FrameWork が呼び出す：次シーンを受け取る（nullptr なら切り替えなし）
    SceneBase* takeNextScene() {
        SceneBase* s = nextScene_;
        nextScene_ = nullptr;
        return s;
    }

protected:
    void quit() { running_ = false; }

    // 派生クラスがシーン切り替えをリクエストする
    void changeScene(SceneBase* next) {
        delete nextScene_;  // 二重リクエストは後勝ち
        nextScene_ = next;
    }

private:
    bool       running_   = true;
    SceneBase* nextScene_ = nullptr;
};

}  // namespace engine
