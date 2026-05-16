#pragma once

#include "Renderer.h"
#include "SceneBase.h"
#include <functional>
#include <vector>

namespace engine {

class ActorBase;

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
    SceneBase*              scene_;
    Renderer                renderer_;
    int                     frameMs_;
    SceneFactory            sceneFactory_;
    std::vector<ActorBase*> actors_;  // 全Scene共有のActor管理用メモリ
};

}  // namespace engine
