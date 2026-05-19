#pragma once

#include "../../Engine/include/SceneBase.h"
#include "../../Engine/include/UIBase.h"
#include "../../Engine/include/InputHandler.h"
#include "../Actor/player.h"
#include "../Actor/bullet.h"
#include "../Actor/invader_swarm.h"
#include "score_manager.h"
#include "game_ui.h"
#include <vector>
#include <list>
#include <ctime>

namespace game {

enum class GameState {
    Playing,
    GameClear
};

class GameScene : public engine::SceneBase {
public:
    explicit GameScene(engine::Allocator& allocator);

    void sceneCalcImpl_() override;
    void draw() override;

private:
    void processInput_();
    void debugKillAllInvaders_();

    GameState    mState;
    int          mLevel;
    std::time_t  mGameStartTime;
    int          mClearCounter;
    int          mLastAliveCount;
    int          mInputCooldown  = 30;  // 入力受け付けクールダウン（フレーム）

    Player*           mPlayer;           // ヒープ確保（Allocator管理）
    engine::InputHandler mInput;
    ScoreManager      mScoreManager;
    std::list<Bullet> mBullets;          // ポインタ安定のため list を使用
    InvaderSwarm      mSwarm;

    // UIBaseへのオブザーバーポインタ（所有権はSceneBase::mUIs_）
    engine::UIBase*   mClearMessageUI = nullptr;
};

}  // namespace game
