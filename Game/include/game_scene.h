#pragma once

#include "../../Engine/include/SceneBase.h"
#include "player.h"
#include "input_handler.h"
#include "bullet.h"
#include "invader_swarm.h"
#include "score_manager.h"
#include <vector>
#include <ctime>

namespace game {

enum class GameState {
    Playing,
    GameClear
};

class GameScene : public engine::SceneBase {
public:
    GameScene();

    void calc() override;
    void draw(engine::Renderer& renderer) override;

private:
    void processInput();
    void debugKillAllInvaders();

    GameState    state_;
    int          level_;
    std::time_t  gameStartTime_;
    int          clearCounter_;
    int          lastAliveCount_;

    Player       player_;
    InputHandler input_;
    ScoreManager scoreManager_;
    std::vector<Bullet> bullets_;
    InvaderSwarm swarm_;
};

}  // namespace game
