#pragma once

#include "config.h"
#include "renderer.h"
#include "player.h"
#include "input_handler.h"
#include "bullet.h"
#include "invader_swarm.h"
#include <vector>

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

    GameState     state_;
    int           level_;
    bool          running_;

    Renderer      renderer_;
    Player        player_;
    InputHandler  input_;
    std::vector<Bullet> bullets_;
    InvaderSwarm  swarm_;
};
