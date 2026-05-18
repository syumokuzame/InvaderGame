#include "../include/SceneBase.h"
#include "../include/ColliderComponent.h"
#include "../include/RenderQueue.h"
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

    // Collision phase: preCalc 完了後に当たり判定を走らせる
    calcCollisions_();

    // Phase 2: postCalc（当たり判定結果を受けて状態変更）
    for (auto* a : mActors_) {
        if (a->isActive()) a->postCalc();
    }
}

void SceneBase::calcCollisions_() {
    // collider() を持つアクターを収集（mActors_ と mColliders_ の両方から）
    std::vector<ActorBase*> collidables;
    for (auto* a : mActors_) {
        if (a->isActive() && a->collider() != nullptr) collidables.push_back(a);
    }
    for (auto* a : mColliders_) {
        if (a->isActive() && a->collider() != nullptr) collidables.push_back(a);
    }

    // 全コンポーネントの hits をリセット
    for (auto* a : collidables) a->collider()->clearHits();

    // 全ペアを総当たりで走査
    const size_t n = collidables.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            ActorBase* a  = collidables[i];
            ActorBase* b  = collidables[j];
            ColliderComponent* ca = a->collider();
            ColliderComponent* cb = b->collider();

            // グループ/マスクが合致する向きのみ判定
            const bool aHitsB = (ca->group() & cb->mask()) != 0;
            const bool bHitsA = (cb->group() & ca->mask()) != 0;
            if (!aHitsB && !bHitsA) continue;

            // セルを展開して重複を探す（最初のヒットで次のペアへ）
            const auto cellsA = ca->worldCells(a->x(), a->y());
            const auto cellsB = cb->worldCells(b->x(), b->y());
            bool hit = false;
            for (const auto& ca_cell : cellsA) {
                if (hit) break;
                for (const auto& cb_cell : cellsB) {
                    if (ca_cell.x == cb_cell.x && ca_cell.y == cb_cell.y) {
                        if (aHitsB) cb->addHit(a, ca_cell.x, ca_cell.y);
                        if (bHitsA) ca->addHit(b, ca_cell.x, ca_cell.y);
                        hit = true;
                        break;
                    }
                }
            }
        }
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

void SceneBase::registerCollider_(ActorBase* actor) {
    mColliders_.push_back(actor);
}

void SceneBase::cleanupColliders_() {
    mColliders_.erase(
        std::remove_if(mColliders_.begin(), mColliders_.end(),
            [](ActorBase* a) { return !a->isActive(); }),
        mColliders_.end()
    );
}

void SceneBase::clearColliders_() {
    mColliders_.clear();
}

void SceneBase::drawBackground_(int width, int height) {
    auto& rq = RenderQueue::instance();
    const int bgLayer = static_cast<int>(RenderLayer::Background);

    // 枠線（Background レイヤー）
    for (int x = 0; x < width; ++x) {
        rq.submit(x, 2, '=', bgLayer);
        rq.submit(x, height - 1, '=', bgLayer);
    }
    for (int y = 3; y < height - 1; ++y) {
        rq.submit(0, y, '|', bgLayer);
        rq.submit(width - 1, y, '|', bgLayer);
    }
}

}  // namespace engine

