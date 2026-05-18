#include "../include/SceneBase.h"
#include <algorithm>

namespace engine {

SceneBase::SceneBase(Allocator& allocator)
    : mAllocator(allocator) {}

UIBase* SceneBase::addUI_(std::unique_ptr<UIBase> ui) {
    auto* ptr = ui.get();
    mUIs_.push_back(std::move(ui));
    return ptr;
}

void SceneBase::calcUIs_() {
    for (auto& ui : mUIs_) ui->calc();
}

void SceneBase::drawUIs_() {
    for (auto& ui : mUIs_) ui->draw();
}

void SceneBase::registerActor_(ActorBase* actor) {
    mActors_.push_back(actor);
}

void SceneBase::calcActors_() {
    // Phase 1: preCalc（移動・アニメーション）
    for (auto* a : mActors_) {
        if (a->isActive()) a->preCalc();
    }
    // Phase 2: postCalc（当たり判定）
    for (auto* a : mActors_) {
        if (a->isActive()) a->postCalc();
    }
}

void SceneBase::cleanupActors_() {
    mActors_.erase(
        std::remove_if(mActors_.begin(), mActors_.end(),
            [](ActorBase* a) { return !a->isActive(); }),
        mActors_.end()
    );
}

void SceneBase::drawActors_() const {
    for (const auto* a : mActors_) {
        if (a->isActive()) a->draw();
    }
}

}  // namespace engine

