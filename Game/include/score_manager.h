#pragma once

#include <string>

namespace game {

class ScoreManager {
public:
    ScoreManager();

    void  addScore(int points);
    int   score()     const { return score_; }
    int   highScore() const { return highScore_; }

    void  loadHighScore();
    void  saveHighScore();
    void  reset();

private:
    int score_;
    int highScore_;
};

}  // namespace game
