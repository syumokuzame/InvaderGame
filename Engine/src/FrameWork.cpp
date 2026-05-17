#include "FrameWork.h"
#include "SceneBase.h"
#include <windows.h>

namespace engine {

FrameWork::FrameWork(int frameMs)
    : scene_(nullptr), frameMs_(frameMs), sceneFactory_(nullptr) {}

FrameWork::~FrameWork() {
    delete scene_;
    // allocator_ はデストラクタで自動解放
}

void FrameWork::run(SceneType initialScene) {
    scene_ = sceneFactory_(initialScene, allocator_);

    while (true) {
        DWORD frameStart = GetTickCount();

        scene_->calc();

        // シーン切り替えチェック（Framework側でシーンを生成）
        if (scene_->hasNextScene()) {
            if (!sceneFactory_) break;

            SceneType nextType = scene_->getNextSceneType();

            // 旧シーンを削除 → Allocatorをクリア → 新シーンを生成
            delete scene_;
            allocator_.clear();
            scene_ = sceneFactory_(nextType, allocator_);
            scene_->clearNextScene();
            continue;
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

