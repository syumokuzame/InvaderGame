#pragma once

#include "Renderer.h"
#include "SceneBase.h"
#include "Allocator.h"
#include <functional>

namespace engine {

class FrameWork {
public:
    explicit FrameWork(int frameMs);
    ~FrameWork();

    // コピー不可
    FrameWork(const FrameWork&) = delete;
    FrameWork& operator=(const FrameWork&) = delete;

    // Game層からシーン生成関数を登録する（Allocatorも渡す）
    using SceneFactory = std::function<SceneBase*(SceneType, Allocator&)>;
    void setSceneFactory(SceneFactory factory) {
        mSceneFactory = std::move(factory);
    }

    // 最初のシーン種別を指定してゲームループを開始する
    void run(SceneType initialScene);

private:
    SceneBase*   mScene;
    Renderer     mRenderer;
    int          mFrameMs;
    SceneFactory mSceneFactory;
    Allocator    mAllocator;  // 全シーンで共有するヒープ管理
};

}  // namespace engine

