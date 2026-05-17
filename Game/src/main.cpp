#include "Scene/title_scene.h"
#include "Scene/game_scene.h"
#include "config.h"
#include "FrameWork.h"
#include <iostream>

int main() {
    try {
        engine::FrameWork fw(Config::FRAME_MS);

        // シーン生成 factory を登録（Allocator は Framework が管理・提供する）
        fw.setSceneFactory([](engine::SceneType type, engine::Allocator& alloc) -> engine::SceneBase* {
            switch (type) {
                case engine::SceneType::Title: return new game::TitleScene(alloc);
                case engine::SceneType::Game:  return new game::GameScene(alloc);
            }
            return nullptr;
        });

        fw.run(engine::SceneType::Title);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unexpected error occurred" << std::endl;
        return 1;
    }
    return 0;
}
