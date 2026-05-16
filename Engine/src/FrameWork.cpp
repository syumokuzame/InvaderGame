#include "FrameWork.h"
#include "SceneBase.h"
#include <windows.h>

namespace engine {

FrameWork::FrameWork(SceneBase* scene, int frameMs)
    : scene_(scene), frameMs_(frameMs) {}

FrameWork::~FrameWork() {
    delete scene_;
}

void FrameWork::run() {
    while (true) {
        DWORD frameStart = GetTickCount();

        scene_->calc();

        // シーン切り替えチェック（isRunning より優先）
        SceneBase* next = scene_->takeNextScene();
        if (next) {
            delete scene_;
            scene_ = next;
            // 新シーンも同じフレームで描画する（continue しない）
        }

        if (!scene_->isRunning()) break;

        renderer_.clear();
        scene_->draw(renderer_);
        renderer_.present();

        DWORD elapsed = GetTickCount() - frameStart;
        if (elapsed < static_cast<DWORD>(frameMs_))
            Sleep(frameMs_ - elapsed);
    }
}

}  // namespace engine
