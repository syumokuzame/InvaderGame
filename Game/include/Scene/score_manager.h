#pragma once

#include <string>

namespace game {

class ScoreManager {
public:
    ScoreManager();

    void  addScore_(int points);
    int   score_()     const { return mScore; }
    int   highScore_() const { return mHighScore; }

    void  loadHighScore_();
    void  saveHighScore_();
    void  reset_();

private:
    int mScore;
    int mHighScore;
};

}  // namespace game
