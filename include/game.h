#pragma once

#include "config.h"
#include "renderer.h"
#include "player.h"
#include "input_handler.h"
#include "bullet.h"
#include "invader_swarm.h"
#include "score_manager.h"
#include <vector>
#include <ctime>

enum class GameState {
    Title,
    Playing,
    Paused,
    GameOver,
    GameClear
};

class Game {
public:
    Game();
    ~Game();

    void run();   // メインループ

private:
    void processInput();
    void update();
    void render();
    void debugKillAllInvaders();  // デバッグ機能：A キーで全敵削除

    GameState     state_;
    int           level_;
    bool          running_;
    std::time_t   gameStartTime_;
    int           clearCounter_;        // Clear状態の表示フレームカウンター
    int           lastAliveCount_;      // 前フレームの生存敵数

    Renderer      renderer_;
    Player        player_;
    InputHandler  input_;
    ScoreManager  scoreManager_;
    std::vector<Bullet> bullets_;
    InvaderSwarm  swarm_;
};
