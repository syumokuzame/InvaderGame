#include "../include/SceneBase.h"

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

}  // namespace engine

