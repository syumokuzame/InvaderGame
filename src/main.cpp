#include "game.h"
#include <iostream>
#include <cstdlib>

int main() {
    try {
        Game game;
        game.run();
    } catch (const std::exception& e) {
        // エラーメッセージを出力して終了
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        // 予期しない例外
        std::cerr << "Unexpected error occurred" << std::endl;
        return 1;
    }
    return 0;
}
