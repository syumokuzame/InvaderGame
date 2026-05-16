#include "../include/SceneBase.h"
#include "../include/ActorBase.h"
#include "../include/Renderer.h"

namespace engine {

SceneBase::SceneBase(std::vector<ActorBase*>& actors)
    : actors_(actors) {}

void SceneBase::updateActors() {
    for (auto actor : actors_) {
        if (actor) actor->calc();
    }
}

void SceneBase::drawActors(Renderer& renderer) {
    for (auto actor : actors_) {
        if (actor) actor->draw(renderer);
    }
}

void SceneBase::clearActors() {
    for (auto actor : actors_) {
        delete actor;
    }
    actors_.clear();
}

}  // namespace engine
