#pragma once
#include "TestInputScript.h"
#include <string>
#include <vector>

namespace engine {

// テストスクリプト設定ファイルローダー
//
// フレームベースの入力イベントと期待ログエントリを設定ファイルから読み込み、
// TestInputScript を構築する。設定ファイルを変更するだけでテストシナリオを
// 追加・変更できる（main.cpp の再編集不要）。
//
// 設定ファイル形式 (test-script.cfg):
//   event <frame> <action>   : 指定フレームでボタン入力を注入
//   expect <log_entry>       : テスト後に debug.log に存在すべき文字列
//   # で始まる行はコメント
//
// アクション名: left, right, shoot, pause, quit, enter, debugKillAll
class TestScriptLoader {
public:
    struct LoadResult {
        TestInputScript          script;
        std::vector<std::string> expectedLogs;
    };

    // cfg ファイルを読み込んでスクリプトと期待ログを返す
    // ファイルが存在しない場合は std::runtime_error を投げる
    static LoadResult load(const std::string& cfgPath);

    // ログファイルの内容を expectedLogs で検証し、Logger に結果を書き込む
    // すべてのエントリが見つかれば true を返す
    static bool validateLogs(const std::string& logPath,
                              const std::vector<std::string>& expectedLogs);
};

} // namespace engine
