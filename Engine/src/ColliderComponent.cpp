#include "../include/ColliderComponent.h"

namespace engine {

ColliderComponent::ColliderComponent(uint32_t group, uint32_t mask)
    : mGroup(group), mMask(mask) {}

void ColliderComponent::addCell(int dx, int dy) {
    mCells.emplace_back(dx, dy);
}

void ColliderComponent::clearHits() {
    mHits.clear();
}

void ColliderComponent::addHit(ActorBase* other, int x, int y) {
    mHits.push_back({other, x, y});
}

std::vector<ColliderComponent::Cell> ColliderComponent::worldCells(int ox, int oy) const {
    std::vector<Cell> result;
    result.reserve(mCells.size());
    for (const auto& [dx, dy] : mCells) {
        result.push_back({ox + dx, oy + dy});
    }
    return result;
}

}  // namespace engine
