#include "FrameWork.h"
#include "SceneBase.h"
#include <windows.h>

namespace engine {

FrameWork::FrameWork(int frameMs)
    : mScene(nullptr), mFrameMs(frameMs), mSceneFactory(nullptr) {}

FrameWork::~FrameWork() {
    delete mScene;
    // mAllocator はデストラクタで自動解放
}

void FrameWork::run(SceneType initialScene) {
    mScene = mSceneFactory(initialScene, mAllocator);

    while (true) {
        DWORD frameStart = GetTickCount();

        mScene->calc();

        // シーン切り替えチェック（Framework側でシーンを生成）
        if (mScene->hasNextScene()) {
            if (!mSceneFactory) break;

            SceneType nextType = mScene->getNextSceneType();

            // 旧シーンを削除 → Allocatorをクリア → 新シーンを生成
            delete mScene;
            mAllocator.clear();
            mScene = mSceneFactory(nextType, mAllocator);
            mScene->clearNextScene();
            continue;
        }

        if (!mScene->isRunning()) break;

        mRenderer.clear();
        mScene->draw(mRenderer);
        mRenderer.present();

        DWORD elapsed = GetTickCount() - frameStart;
        if (elapsed < static_cast<DWORD>(mFrameMs))
            Sleep(mFrameMs - elapsed);
    }
}

}  // namespace engine

