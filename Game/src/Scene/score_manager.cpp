#include "Scene/score_manager.h"
#include "config.h"
#include <fstream>

namespace game {

ScoreManager::ScoreManager()
    : score_(0), highScore_(0) {
    loadHighScore();
}

void ScoreManager::addScore(int points) {
    score_ += points;
    if (score_ > highScore_) {
        highScore_ = score_;
        saveHighScore();
    }
}

void ScoreManager::loadHighScore() {
    std::ifstream file(Config::HIGHSCORE_FILE, std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&highScore_), sizeof(int));
        file.close();
    }
}

void ScoreManager::saveHighScore() {
    std::ofstream file(Config::HIGHSCORE_FILE, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&highScore_), sizeof(int));
        file.close();
    }
}

void ScoreManager::reset() {
    score_ = 0;
}

}  // namespace game
