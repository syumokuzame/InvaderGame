#include "game.h"

int main() {
    try {
        Game game;
        game.run();
    } catch (...) {
        // 予期しない例外が発生してもコンソール設定が復元される
    }
    return 0;
}
