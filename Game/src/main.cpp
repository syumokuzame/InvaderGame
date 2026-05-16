#include "Scene/title_scene.h"
#include "Scene/game_scene.h"
#include "config.h"
#include "FrameWork.h"
#include <iostream>

int main() {
    try {
        // Actor管理用メモリを先に確保
        std::vector<engine::ActorBase*> actors;

        // シーン生成用の factory を登録（actors への参照をキャプチャ）
        auto sceneFactory = [&actors](engine::SceneType type) -> engine::SceneBase* {
            switch (type) {
                case engine::SceneType::Title:
                    return new game::TitleScene(actors);
                case engine::SceneType::Game:
                    return new game::GameScene(actors);
            }
            return nullptr;
        };

        engine::FrameWork fw(new game::TitleScene(actors), Config::FRAME_MS);
        fw.setSceneFactory(sceneFactory);
        fw.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unexpected error occurred" << std::endl;
        return 1;
    }
    return 0;
}
