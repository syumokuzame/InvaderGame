#include "ActorBase.h"

namespace engine {

ActorBase::ActorBase(int x, int y)
    : mX(x), mY(y) {
}

void ActorBase::draw() const {
    if (mModel.empty()) return;
    RenderQueue::instance().submitModel(mX, mY, mModel, drawLayer());
}

}  // namespace engine

