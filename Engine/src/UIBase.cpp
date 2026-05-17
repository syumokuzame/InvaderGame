#include "UIBase.h"

namespace engine {

UIBase::UIBase(int x, int y, std::string text)
    : mX(x), mY(y), mText(std::move(text)) {}

void UIBase::draw() const {
    if (!isVisible() || mText.empty()) return;
    RenderQueue::instance().submitString(mX, mY, mText.c_str());
}

}  // namespace engine
