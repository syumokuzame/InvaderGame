#pragma once
#include "InputHandler.h"
#include <functional>
#include <vector>

namespace engine {

// フレームベースのテスト入力スクリプト
//
// Game 層の main.cpp でイベントを組み立て、InputHandler に登録する。
// スクリプトの「内容」（どのフレームで何を押すか）は Game 層が担い、
// 「仕組み」（フレームカウント・イベント照合・ボタン注入）は Engine 層が担う。
//
// 使用例:
//   engine::TestInputScript script;
//   script.addEvent(65,  [](engine::InputHandler& in) { in.setEnter_(true); });
//   script.addEvent(130, [](engine::InputHandler& in) { in.setShoot_(true); });
//   engine::InputHandler::setTestProvider(script.createProvider());
class TestInputScript {
public:
    using SetterFn = std::function<void(InputHandler&)>;

    // 指定フレームで実行するボタン注入イベントを追加する
    // 同一フレームに複数登録可能
    void addEvent(int frame, SetterFn setter);

    // InputHandler::setTestProvider() に渡せるプロバイダーを生成する
    // createProvider() 後に addEvent() を呼んでも反映されない（スナップショット）
    InputHandler::TestProvider createProvider() const;

private:
    struct Event {
        int      frame;
        SetterFn setter;
    };
    std::vector<Event> mEvents;
};

}  // namespace engine
