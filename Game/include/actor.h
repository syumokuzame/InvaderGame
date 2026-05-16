#pragma once

#include "../../Engine/include/ActorBase.h"

namespace game {

class Renderer;

/**
 * @class Actor
 * @brief Game層のアクター基底クラス
 * Engine層の ActorBase を継承
 */
class Actor : public engine::ActorBase {
public:
    Actor(int x, int y);
    virtual ~Actor() = default;

    // ActorBase の仮想メソッドを継承
    // 具体的な実装はサブクラスで定義
};

}  // namespace game
