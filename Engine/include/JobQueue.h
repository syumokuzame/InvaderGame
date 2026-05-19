#pragma once

#include <functional>
#include <queue>
#include <vector>

namespace engine {

// 優先度付きジョブキュー
// enqueue() でジョブと優先度を登録し、flush() で優先度順（昇順）に一括実行する。
// 同優先度のジョブは登録順（FIFO）で実行される。
// flush() 後はキューが空になる。
class JobQueue {
public:
    // fn       : 実行するジョブ
    // priority : 実行優先度。値が小さいほど先に実行される（デフォルト 0）
    void enqueue(std::function<void()> fn, int priority = 0);

    // キューに積まれた全ジョブを優先度順に実行してキューをクリアする
    void flush();

    bool empty() const { return mQueue_.empty(); }

private:
    struct Job {
        int priority;
        int seq;  // 同優先度内での FIFO 保証用シーケンス番号
        std::function<void()> fn;

        // std::greater<Job> で最小ヒープ（優先度昇順）を構成する
        bool operator>(const Job& other) const {
            if (priority != other.priority) return priority > other.priority;
            return seq > other.seq;
        }
    };

    using PQueue = std::priority_queue<Job, std::vector<Job>, std::greater<Job>>;
    PQueue mQueue_;
    int    mSeq_ = 0;
};

}  // namespace engine
