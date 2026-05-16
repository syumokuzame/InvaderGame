#include "game_scene.h"
#include "title_scene.h"
#include "../../Engine/include/Renderer.h"
#include "config.h"
#include <algorithm>
#include <ctime>

namespace game {

GameScene::GameScene()
    : state_(GameState::Playing), level_(1), clearCounter_(0), lastAliveCount_(0) {
    // 前シーンからのキー入力状態をリセット
    input_.poll();
    gameStartTime_ = std::time(nullptr);
    for (const auto& inv : swarm_.invaders()) {
        if (inv.isActive()) lastAliveCount_++;
    }
}

void GameScene::processInput() {
    input_.poll();

    if (input_.isQuit())  { changeScene(new TitleScene()); return; }
    if (input_.isLeft())  player_.moveLeft();
    if (input_.isRight()) player_.moveRight();
    if (input_.isShoot()) {
        if (player_.shoot()) {
            bullets_.emplace_back(player_.x(), player_.y() - 1, BulletOwner::Player);
        }
    }
    if (input_.isDebugKillAll()) {
        debugKillAllInvaders();
    }
}

void GameScene::calc() {
    processInput();

    player_.calc();

    for (auto& b : bullets_) {
        b.calc();
    }

    if (state_ == GameState::Playing) {
        swarm_.update(bullets_);

        int currentAliveCount = 0;
        for (const auto& inv : swarm_.invaders()) {
            if (inv.isActive()) currentAliveCount++;
        }
        int defeatedCount = lastAliveCount_ - currentAliveCount;
        if (defeatedCount > 0) {
            scoreManager_.addScore(defeatedCount * 10);
        }
        lastAliveCount_ = currentAliveCount;

        if (swarm_.allDefeated()) {
            state_ = GameState::GameClear;
            clearCounter_ = 0;
        }
    } else if (state_ == GameState::GameClear) {
        clearCounter_++;
        if (clearCounter_ >= 60) {
            swarm_.reset(level_);
            lastAliveCount_ = 0;
            for (const auto& inv : swarm_.invaders()) {
                if (inv.isActive()) lastAliveCount_++;
            }
            bullets_.clear();
            player_.clearBullet();
            state_ = GameState::Playing;
        }
    }

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

void GameScene::draw(engine::Renderer& renderer) {
    std::time_t now = std::time(nullptr);
    int elapsedSeconds = static_cast<int>(now - gameStartTime_);
    renderer.drawHeader("InvaderGame", scoreManager_.score(), elapsedSeconds);
    renderer.drawInstructions();

    for (int x = 0; x < Config::FIELD_WIDTH; ++x) {
        renderer.draw(x, 2, '=');
        renderer.draw(x, Config::FIELD_HEIGHT - 1, '=');
    }
    for (int y = 3; y < Config::FIELD_HEIGHT - 1; ++y) {
        renderer.draw(0, y, '|');
        renderer.draw(Config::FIELD_WIDTH - 1, y, '|');
    }

    for (const auto& b : bullets_)
        b.draw(renderer);

    swarm_.draw(renderer);
    player_.draw(renderer);

    if (state_ == GameState::GameClear) {
        int msgY = Config::FIELD_HEIGHT / 2;
        int msgX = Config::FIELD_WIDTH / 2 - 2;
        renderer.drawString(msgX, msgY, "CLEAR!");
    }
}

void GameScene::debugKillAllInvaders() {
    for (auto& inv : swarm_.invaders()) {
        if (!inv.isFullyDead()) {
            inv.kill();
        }
    }
}

}  // namespace game
