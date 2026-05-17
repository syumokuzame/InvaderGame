#include "Scene/score_manager.h"
#include "config.h"
#include <fstream>

namespace game {

ScoreManager::ScoreManager()
    : mScore(0), mHighScore(0) {
    loadHighScore_();
}

void ScoreManager::addScore_(int points) {
    mScore += points;
    if (mScore > mHighScore) {
        mHighScore = mScore;
        saveHighScore_();
    }
}

void ScoreManager::loadHighScore_() {
    std::ifstream file(Config::HIGHSCORE_FILE, std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&mHighScore), sizeof(int));
        file.close();
    }
}

void ScoreManager::saveHighScore_() {
    std::ofstream file(Config::HIGHSCORE_FILE, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&mHighScore), sizeof(int));
        file.close();
    }
}

void ScoreManager::reset_() {
    mScore = 0;
}

}  // namespace game
