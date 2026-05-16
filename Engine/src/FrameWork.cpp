#include "FrameWork.h"
#include "SceneBase.h"
#include <windows.h>

namespace engine {

FrameWork::FrameWork(SceneBase* scene, int frameMs)
    : scene_(scene), frameMs_(frameMs) {}

void FrameWork::run() {
    while (scene_->isRunning()) {
        DWORD frameStart = GetTickCount();

        scene_->calc();

        renderer_.clear();
        scene_->draw(renderer_);
        renderer_.present();

        DWORD elapsed = GetTickCount() - frameStart;
        if (elapsed < static_cast<DWORD>(frameMs_))
            Sleep(frameMs_ - elapsed);
    }
}

}  // namespace engine
