#pragma once

#include "Renderer.h"
#include "SceneBase.h"
#include <functional>

namespace engine {

class FrameWork {
public:
    // scene の所有権を FrameWork が持つ（delete する）
    FrameWork(SceneBase* scene, int frameMs);
    ~FrameWork();

    // コピー不可
    FrameWork(const FrameWork&) = delete;
    FrameWork& operator=(const FrameWork&) = delete;

    // Game層からシーン生成関数を登録する
    using SceneFactory = std::function<SceneBase*(SceneType)>;
    void setSceneFactory(SceneFactory factory) {
        sceneFactory_ = factory;
    }

    void run();

private:
    SceneBase*  scene_;
    Renderer    renderer_;
    int         frameMs_;
    SceneFactory sceneFactory_;
};

}  // namespace engine
