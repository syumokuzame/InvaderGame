#include "game_scene.h"
#include "config.h"
#include "FrameWork.h"
#include <iostream>

int main() {
    try {
        game::GameScene scene;
        engine::FrameWork fw(&scene, Config::FRAME_MS);
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
