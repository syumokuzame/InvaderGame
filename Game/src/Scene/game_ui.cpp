#include "Scene/game_ui.h"
#include <cstdio>
#include <ctime>

namespace game {

// --- ScoreUI ---

ScoreUI::ScoreUI(int x, int y, const ScoreManager& sm)
    : UIBase(x, y), mScoreManager(sm) {}

void ScoreUI::calc() {
    char buf[32];
    sprintf_s(buf, sizeof(buf), "Score: %d", mScoreManager.score_());
    setText(buf);
}

// --- TimerUI ---

TimerUI::TimerUI(int x, int y, std::time_t startTime)
    : UIBase(x, y), mStartTime(startTime) {}

void TimerUI::calc() {
    int elapsed = static_cast<int>(std::time(nullptr) - mStartTime);
    char buf[16];
    sprintf_s(buf, sizeof(buf), "%02d:%02d", elapsed / 60, elapsed % 60);
    setText(buf);
}

}  // namespace game
