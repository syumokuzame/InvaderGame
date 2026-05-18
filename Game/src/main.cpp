#include "Scene/title_scene.h"
#include "Scene/game_scene.h"
#include "config.h"
#include "FrameWork.h"
#include "Logger.h"
#include "InputHandler.h"
#include "TestScriptLoader.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // --test 引数でテストモード起動: 設定ファイルからスクリプトを読み込み自動終了
    bool testMode = (argc > 1 && std::string(argv[1]) == "--test");

    try {
        engine::TestScriptLoader::LoadResult testData;
        if (testMode) {
            testData = engine::TestScriptLoader::load("Game/docs/specs/test-script.cfg");
            engine::InputHandler::setTestProvider(testData.script.createProvider());
            engine::Logger::instance().init("save/debug.log");
        }

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
            bool allPass = engine::TestScriptLoader::validateLogs(
                "save/debug.log", testData.expectedLogs);
            return allPass ? 0 : 1;
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
