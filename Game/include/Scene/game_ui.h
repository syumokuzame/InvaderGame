#pragma once

#include "../../Engine/include/UIBase.h"
#include "score_manager.h"
#include <ctime>

namespace game {

// スコアを動的に表示するUI
class ScoreUI : public engine::UIBase {
public:
    ScoreUI(int x, int y, const ScoreManager& sm);
    void calc() override;  // mText をスコア文字列で更新

private:
    const ScoreManager& mScoreManager;
};

// 経過時間を動的に表示するUI
class TimerUI : public engine::UIBase {
public:
    TimerUI(int x, int y, std::time_t startTime);
    void calc() override;  // mText を MM:SS 形式で更新

private:
    std::time_t mStartTime;
};

}  // namespace game
