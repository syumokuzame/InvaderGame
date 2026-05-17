#pragma once

#include <fstream>
#include <string>

namespace engine {

// シングルトンの簡易ファイルロガー
// テストモード時に save/debug.log へキーイベントを書き出す
class Logger {
public:
    static Logger& instance();

    // ゲーム起動時に一度呼ぶ。ファイルをリセットして "[GAME] Start" を書く
    void init(const std::string& path);

    // メッセージを1行書き込む（即フラッシュ）
    void log(const std::string& message);

private:
    Logger() = default;
    std::ofstream mFile;
};

} // namespace engine
