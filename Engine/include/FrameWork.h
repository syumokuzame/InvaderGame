#pragma once

#include "Renderer.h"

namespace engine {

class SceneBase;

class FrameWork {
public:
    FrameWork(SceneBase* scene, int frameMs);
    void run();

private:
    SceneBase* scene_;
    Renderer   renderer_;
    int        frameMs_;
};

}  // namespace engine
