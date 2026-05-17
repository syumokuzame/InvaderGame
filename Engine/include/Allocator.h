#pragma once

#include <vector>

namespace engine {

// 汎用ヒープアロケーター
// create<T>() で任意の型をヒープ確保し、所有権を持つ
// clear() / デストラクタで一括解放する
class Allocator {
public:
    Allocator() = default;
    ~Allocator() { clear(); }

    // コピー不可
    Allocator(const Allocator&) = delete;
    Allocator& operator=(const Allocator&) = delete;

    // T を new して管理下に置き、ポインタを返す
    template<typename T, typename... Args>
    T* create(Args&&... args) {
        T* ptr = new T(std::forward<Args>(args)...);
        entries_.push_back({ static_cast<void*>(ptr),
                             [](void* p){ delete static_cast<T*>(p); } });
        return ptr;
    }

    // 管理下の全オブジェクトを逆順で delete する
    void clear() {
        for (auto it = entries_.rbegin(); it != entries_.rend(); ++it) {
            it->del(it->ptr);
        }
        entries_.clear();
    }

private:
    struct Entry {
        void*  ptr;
        void (*del)(void*);
    };

    std::vector<Entry> entries_;
};

}  // namespace engine
