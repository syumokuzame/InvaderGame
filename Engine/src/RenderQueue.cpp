#include "RenderQueue.h"
#include "Renderer.h"
#include <algorithm>

namespace engine {

RenderQueue& RenderQueue::instance() {
    static RenderQueue inst;
    return inst;
}

void RenderQueue::clear() {
    mCommands.clear();
}

void RenderQueue::submit(int x, int y, char ch, int layer) {
    mCommands.push_back({ x, y, std::string(1, ch), layer });
}

void RenderQueue::submitString(int x, int y, const char* str, int layer) {
    mCommands.push_back({ x, y, std::string(str), layer });
}

void RenderQueue::flush(Renderer& renderer) {
    std::stable_sort(mCommands.begin(), mCommands.end(),
        [](const Command& a, const Command& b) { return a.layer < b.layer; });

    for (const auto& cmd : mCommands) {
        if (cmd.str.size() == 1) {
            renderer.draw(cmd.x, cmd.y, cmd.str[0]);
        } else {
            renderer.drawString(cmd.x, cmd.y, cmd.str.c_str());
        }
    }
}

}  // namespace engine
