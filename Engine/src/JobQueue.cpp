#include "../include/JobQueue.h"

namespace engine {

void JobQueue::enqueue(std::function<void()> fn, int priority) {
    mQueue_.push({priority, mSeq_++, std::move(fn)});
}

void JobQueue::flush() {
    while (!mQueue_.empty()) {
        // top() はコピーで取り出す（pop() で参照が無効になるため）
        auto job = mQueue_.top();
        mQueue_.pop();
        job.fn();
    }
}

}  // namespace engine
