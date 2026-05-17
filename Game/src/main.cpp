#include "Scene/title_scene.h"
#include "Scene/game_scene.h"
#include "config.h"
#include "FrameWork.h"
#include "Logger.h"
#include "input_handler.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // --test 引数でテストモード起動: スクリプト入力を再生しログを記録して自動終了
    bool testMode = (argc > 1 && std::string(argv[1]) == "--test");
    if (testMode) {
        game::InputHandler::enableTestMode();
        engine::Logger::instance().init("save/debug.log");
    }

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
        if (testMode) {
            engine::Logger::instance().log("[GAME] End");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unexpected error occurred" << std::endl;
        return 1;
    }
    return 0;
}
