#pragma once

#include <list>
#include <mutex>
#include <optional>
namespace RCo {
template <typename T> class RList {
public:
    RList() {
    }
    void push_back(T value) noexcept {
        lock.lock();
        list.push_back(value);
        lock.unlock();
    }
    std::optional<T> pop_front() noexcept {
        lock.lock();
        if (list.empty()) {
            return std::nullopt;
        }
        auto ret = list.front();
        list.pop_front();
        return std::make_optional(ret);
    }

private:
    std::mutex   lock;
    std::list<T> list;
};
} // namespace RCo