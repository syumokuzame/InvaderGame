#include "TestScriptLoader.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <functional>

namespace engine {

namespace {

// アクション名 → InputHandler セッター のマッピング
// InputHandler.h の INPUT_BUTTONS X マクロと同期して保守する
const std::unordered_map<std::string, TestInputScript::SetterFn>& actionMap() {
    static const std::unordered_map<std::string, TestInputScript::SetterFn> kMap = {
        {"left",         [](InputHandler& in) { in.setLeft_(true);         }},
        {"right",        [](InputHandler& in) { in.setRight_(true);        }},
        {"shoot",        [](InputHandler& in) { in.setShoot_(true);        }},
        {"pause",        [](InputHandler& in) { in.setPause_(true);        }},
        {"quit",         [](InputHandler& in) { in.setQuit_(true);         }},
        {"enter",        [](InputHandler& in) { in.setEnter_(true);        }},
        {"debugKillAll", [](InputHandler& in) { in.setDebugKillAll_(true); }},
    };
    return kMap;
}

} // anonymous namespace

TestScriptLoader::LoadResult TestScriptLoader::load(const std::string& cfgPath) {
    std::ifstream file(cfgPath);
    if (!file) {
        throw std::runtime_error("TestScriptLoader: cannot open cfg: " + cfgPath);
    }

    LoadResult result;
    std::string line;
    int lineNum = 0;

    while (std::getline(file, line)) {
        ++lineNum;

        // 先頭の空白を除去
        auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;  // 空行
        line = line.substr(start);

        // コメント行をスキップ
        if (line[0] == '#') continue;

        std::istringstream ss(line);
        std::string keyword;
        ss >> keyword;

        if (keyword == "event") {
            int frame;
            std::string action;
            if (!(ss >> frame >> action)) {
                throw std::runtime_error(
                    "TestScriptLoader: malformed event at line " + std::to_string(lineNum));
            }
            const auto& map = actionMap();
            auto it = map.find(action);
            if (it == map.end()) {
                throw std::runtime_error(
                    "TestScriptLoader: unknown action '" + action +
                    "' at line " + std::to_string(lineNum));
            }
            result.script.addEvent(frame, it->second);

        } else if (keyword == "expect") {
            // "expect " の後の残り全体を期待ログ文字列とする
            std::string entry;
            std::getline(ss, entry);
            auto pos = entry.find_first_not_of(' ');
            if (pos != std::string::npos) {
                result.expectedLogs.push_back(entry.substr(pos));
            }
        }
        // 不明なキーワードは無視（将来の拡張性のため）
    }

    return result;
}

bool TestScriptLoader::validateLogs(const std::string& logPath,
                                    const std::vector<std::string>& expectedLogs) {
    // Logger がファイルを開いたまま書き込んでいる状態で別ハンドルで読む
    // Logger::log() は即フラッシュするため内容は一貫している
    std::ifstream file(logPath);
    if (!file) {
        Logger::instance().log("[TEST] ERROR: cannot open log: " + logPath);
        return false;
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();

    bool allPass = true;
    for (const auto& entry : expectedLogs) {
        if (content.find(entry) != std::string::npos) {
            Logger::instance().log("[TEST] PASS: " + entry);
        } else {
            Logger::instance().log("[TEST] FAIL: " + entry);
            allPass = false;
        }
    }
    return allPass;
}

} // namespace engine
