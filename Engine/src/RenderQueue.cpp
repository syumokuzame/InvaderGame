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
    mModelCommands.clear();
}

void RenderQueue::submit(int x, int y, char ch, int layer) {
    mCommands.push_back({ x, y, std::string(1, ch), layer });
}

void RenderQueue::submitString(int x, int y, const char* str, int layer) {
    mCommands.push_back({ x, y, std::string(str), layer });
}

void RenderQueue::submitModel(int x, int y, const ActorModel& model, int layer) {
    if (model.empty()) return;
    mModelCommands.push_back({ x, y, model, layer });
}

void RenderQueue::flush(Renderer& renderer) {
    // Command と ModelCommand をレイヤー順に処理するため
    // ModelCommand を展開して Command のリストにマージする
    std::vector<Command> all = mCommands;

    for (const auto& mc : mModelCommands) {
        const auto& m = mc.model;
        for (int row = 0; row < m.height(); ++row) {
            for (int col = 0; col < m.width(); ++col) {
                char ch = m.at(row, col);
                // ' ' (スペース) は透過扱い
                if (ch == ' ' || ch == '\0') continue;
                int wx = mc.x + (col - m.pivotX);
                int wy = mc.y + (row - m.pivotY);
                all.push_back({ wx, wy, std::string(1, ch), mc.layer });
            }
        }
    }

    std::stable_sort(all.begin(), all.end(),
        [](const Command& a, const Command& b) { return a.layer < b.layer; });

    for (const auto& cmd : all) {
        if (cmd.str.size() == 1) {
            renderer.draw(cmd.x, cmd.y, cmd.str[0]);
        } else {
            renderer.drawString(cmd.x, cmd.y, cmd.str.c_str());
        }
    }
}

}  // namespace engine
