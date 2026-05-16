#pragma once

#include "../../Engine/include/SceneBase.h"
#include "../Actor/player.h"
#include "../input_handler.h"
#include "../Actor/bullet.h"
#include "../Actor/invader_swarm.h"
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
    explicit GameScene(std::vector<engine::ActorBase*>& actors);
    ~GameScene() override;

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
    int          inputCooldown_  = 30;  // 入力受け付けクールダウン（フレーム）

    Player*      player_;           // ヒープ確保（actors_管理）
    InputHandler input_;
    ScoreManager scoreManager_;
    std::vector<Bullet> bullets_;   // ローカル管理
    InvaderSwarm swarm_;
};

}  // namespace game
