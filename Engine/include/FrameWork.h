#pragma once

#include "Renderer.h"

namespace engine {

class SceneBase;

class FrameWork {
public:
    // scene の所有権を FrameWork が持つ（delete する）
    FrameWork(SceneBase* scene, int frameMs);
    ~FrameWork();

    // コピー不可
    FrameWork(const FrameWork&) = delete;
    FrameWork& operator=(const FrameWork&) = delete;

    void run();

private:
    SceneBase* scene_;
    Renderer   renderer_;
    int        frameMs_;
};

}  // namespace engine
