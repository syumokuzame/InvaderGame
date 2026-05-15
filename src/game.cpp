#include "game.h"
#include "config.h"
#include <windows.h>
#include <algorithm>
#include <ctime>

Game::Game()
    : state_(GameState::Playing), level_(1), running_(true) {
    gameStartTime_ = std::time(nullptr);
}

Game::~Game() = default;

void Game::run() {
    while (running_) {
        DWORD frameStart = GetTickCount();

        processInput();
        update();
        render();

        // フレームレート制御
        DWORD elapsed = GetTickCount() - frameStart;
        if (elapsed < static_cast<DWORD>(Config::FRAME_MS))
            Sleep(Config::FRAME_MS - elapsed);
    }
}

void Game::processInput() {
    input_.poll();

    if (input_.isQuit())  { running_ = false; return; }
    if (input_.isLeft())  player_.moveLeft();
    if (input_.isRight()) player_.moveRight();
    if (input_.isShoot()) {
        if (player_.shoot()) {
            // プレイヤーの中心座標から発射
            bullets_.emplace_back(player_.x(), player_.y() - 1, BulletOwner::Player);
        }
    }
}

void Game::update() {
    player_.update();

    // 弾を更新
    for (auto& b : bullets_) {
        b.update();
    }

    // インベーダー群を更新（弾との当たり判定含む）
    swarm_.update(bullets_);

    // 非アクティブな弾を除去し、プレイヤー弾フラグもリセット
    bullets_.erase(
        std::remove_if(bullets_.begin(), bullets_.end(),
            [this](const Bullet& b) {
                if (!b.isActive()) {
                    if (b.owner() == BulletOwner::Player)
                        player_.clearBullet();
                    return true;
                }
                return false;
            }),
        bullets_.end()
    );
}

void Game::render() {
    renderer_.clear();

    // 上部UI描画（1-2行目）
    std::time_t now = std::time(nullptr);
    int elapsedSeconds = static_cast<int>(now - gameStartTime_);
    renderer_.drawHeader("InvaderGame", scoreManager_.score(), elapsedSeconds);
    renderer_.drawInstructions();

    // フィールド枠（上下）
    for (int x = 0; x < Config::FIELD_WIDTH; ++x) {
        renderer_.draw(x, 2, '=');
        renderer_.draw(x, Config::FIELD_HEIGHT - 1, '=');
    }
    // フィールド枠（左右）
    for (int y = 3; y < Config::FIELD_HEIGHT - 1; ++y) {
        renderer_.draw(0, y, '|');
        renderer_.draw(Config::FIELD_WIDTH - 1, y, '|');
    }

    // 弾を描画
    for (const auto& b : bullets_)
        b.render(renderer_);

    // インベーダー群を描画
    swarm_.render(renderer_);

    // プレイヤー描画
    player_.render(renderer_);

    renderer_.present();
}
