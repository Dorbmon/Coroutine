#pragma once
#include <concepts>
#include <coroutine>
#include <utility>
namespace RCo {
class Core;
class AbstractTask {};
template <typename Result>
class RTask {
 public:
  class RPromise;
  using promise_type = RPromise;
  RTask(std::coroutine_handle<promise_type>, Core* core);
  Core* core;
  void await_suspend(std::coroutine_handle<> h);

 private:
  std::coroutine_handle<RTask<Result>::promise_type> handler;
};
template <typename Result>
class RPromise {
  RTask<Result> get_return_object();
  std::suspend_always initial_suspend() {
    return {};
  }  // never suspend initallly

  template <typename T>
    requires std::convertible_to<T, Result>
  std::suspend_always yield_value(T&& value) {
    // value_ = std::forward<T>(value);  // caching the result in promise
    return {};
  }
};
}  // namespace RCo