#pragma once

namespace engine {

class Renderer;

class SceneBase {
public:
    virtual ~SceneBase() = default;
    virtual void calc() = 0;
    virtual void draw(Renderer& renderer) = 0;

    bool isRunning() const { return running_; }

protected:
    void quit() { running_ = false; }

private:
    bool running_ = true;
};

}  // namespace engine
