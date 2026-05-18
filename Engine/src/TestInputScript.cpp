#include "TestInputScript.h"

namespace engine {

void TestInputScript::addEvent(int frame, SetterFn setter) {
    mEvents.push_back({frame, std::move(setter)});
}

InputHandler::TestProvider TestInputScript::createProvider() const {
    // イベントリストをコピーしてラムダにキャプチャする
    // (スナップショット: 以降の addEvent() は反映されない)
    auto events = mEvents;
    return [events](int frame, InputHandler& input) {
        for (const auto& ev : events) {
            if (ev.frame == frame) {
                ev.setter(input);
            }
        }
    };
}

}  // namespace engine
