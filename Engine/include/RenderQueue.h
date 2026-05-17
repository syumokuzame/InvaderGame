#pragma once

#include <vector>
#include <string>

namespace engine {

class Renderer;

// 描画レイヤー（z-order。数値が小さいほど先に描画）
enum class RenderLayer : int {
    Background = 0,
    Actor      = 10,
    Bullet     = 20,
    UI         = 30,
};

// フレームバッファへの描画要求を蓄積し、フレーム末に一括フラッシュするキュー
class RenderQueue {
public:
    // シングルトン取得（単一スレッドゲームを前提）
    static RenderQueue& instance();

    // フレーム先頭で呼ぶ：前フレームのコマンドを全消去
    void clear();

    // 1文字の描画登録
    void submit(int x, int y, char ch,
                int layer = static_cast<int>(RenderLayer::Actor));

    // 文字列の描画登録（動的生成文字列も安全に保持するため内部でコピー）
    void submitString(int x, int y, const char* str,
                      int layer = static_cast<int>(RenderLayer::UI));

    // フレーム末に呼ぶ：layer 昇順でソートして Renderer へ一括書き込み
    void flush(Renderer& renderer);

private:
    RenderQueue() = default;

    struct Command {
        int         x, y;
        std::string str;   // 1文字の場合も含めて std::string で統一
        int         layer;
    };

    std::vector<Command> mCommands;
};

}  // namespace engine
