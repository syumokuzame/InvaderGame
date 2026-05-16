#include "game.h"
#include "config.h"
#include <windows.h>
#include <algorithm>
#include <ctime>

namespace game {

Game::Game()
    : state_(GameState::Playing), level_(1), running_(true), clearCounter_(0), lastAliveCount_(0) {
    gameStartTime_ = std::time(nullptr);
    // 初期敵数をカウント
    for (const auto& inv : swarm_.invaders()) {
        if (inv.isActive()) lastAliveCount_++;
    }
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
    // デバッグ：A キーで全敵削除
    if (input_.isDebugKillAll()) {
        debugKillAllInvaders();
    }
}

void Game::update() {
    // プレイヤーと弾の更新はゲーム状態に関わらず常に実行
    player_.update();

    // 弾を更新
    for (auto& b : bullets_) {
        b.update();
    }

    if (state_ == GameState::Playing) {
        // インベーダー群を更新（弾との当たり判定含む）
        swarm_.update(bullets_);

        // 敵撃破スコア加算処理
        int currentAliveCount = 0;
        for (const auto& inv : swarm_.invaders()) {
            if (inv.isActive()) currentAliveCount++;
        }
        int defeatedCount = lastAliveCount_ - currentAliveCount;
        if (defeatedCount > 0) {
            scoreManager_.addScore(defeatedCount * 10);  // 1体10点
        }
        lastAliveCount_ = currentAliveCount;

        // 敵全滅判定
        if (swarm_.allDefeated()) {
            state_ = GameState::GameClear;
            clearCounter_ = 0;
        }
    } else if (state_ == GameState::GameClear) {
        // Clear状態の処理
        clearCounter_++;
        if (clearCounter_ >= 60) {  // 1秒（60フレーム）待機
            // 敵をリスポーン
            swarm_.reset(level_);
            lastAliveCount_ = 0;
            for (const auto& inv : swarm_.invaders()) {
                if (inv.isActive()) lastAliveCount_++;
            }
            // 弾をクリア
            bullets_.clear();
            player_.clearBullet();
            // ゲーム状態を Playing に戻す
            state_ = GameState::Playing;
        }
    }

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

    // Clear メッセージ表示
    if (state_ == GameState::GameClear) {
        int msgY = Config::FIELD_HEIGHT / 2;
        int msgX = Config::FIELD_WIDTH / 2 - 2;
        renderer_.drawString(msgX, msgY, "CLEAR!");
    }

    renderer_.present();
}

void Game::debugKillAllInvaders() {
    // 全敵を削除
    for (auto& inv : swarm_.invaders()) {
        if (!inv.isFullyDead()) {
            inv.kill();
        }
    }
}

}  // namespace game
