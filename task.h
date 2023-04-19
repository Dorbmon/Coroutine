#pragma once
#include <concepts>
#include <coroutine>
#include <future>
#include <utility>
namespace RCo {
class RWorker;
class AbstractTask {};
template <typename Result>
class RPromise;
template <typename Result>

class RTask {
 public:
  using promise_type = RPromise<Result>;
  RTask(std::coroutine_handle<promise_type> handler, RWorker* worker);
  RWorker* worker;
  std::promise<Result> promise;
  void await_suspend(std::coroutine_handle<RPromise<Result>> h);
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
  template <typename T>
    requires std::convertible_to<T, Result>
  void return_value(T&& value) noexcept {
    return;
  }
};
}  // namespace RCo