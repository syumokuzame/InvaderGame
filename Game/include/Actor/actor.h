#pragma once

#include "ActorBase.h"

namespace game {

class Actor : public engine::ActorBase {
public:
    Actor(int x, int y);
    virtual ~Actor() = default;

};

}  // namespace game
