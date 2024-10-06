#pragma once

#include <functional>

namespace RCo {

class RWorker;
RWorker *__getCurrentWorker();

template <typename F, typename RET_V>
concept FunctionWithCallback =
    requires(F f, std::function<void(RET_V)> *callback) {
      { f(std::move(callback)) };
    };
template <typename F>
concept FunctionWithVoidCallback =
    requires(F f, std::function<void(void)> *callback) {
      { f(std::move(callback)) };
    };
} // namespace RCo