#include "FrameWork.h"
#include "SceneBase.h"
#include <windows.h>

namespace engine {

FrameWork::FrameWork(SceneBase* scene, int frameMs)
    : scene_(scene), frameMs_(frameMs), sceneFactory_(nullptr) {}

FrameWork::~FrameWork() {
    delete scene_;
}

void FrameWork::run() {
    while (true) {
        DWORD frameStart = GetTickCount();

        scene_->calc();

        // シーン切り替えチェック（Framework側でシーンを生成）
        if (scene_->hasNextScene()) {
            if (!sceneFactory_) {
                // factory が登録されていない場合はエラー
                break;
            }
            
            SceneType nextType = scene_->getNextSceneType();
            SceneBase* nextScene = sceneFactory_(nextType);
            
            delete scene_;
            scene_ = nextScene;
            scene_->clearNextScene();  // 新シーンの切り替え要求をリセット
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
