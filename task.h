#pragma once
#include <concepts>
#include <coroutine>
#include <exception>
#include <future>
#include <utility>
namespace RCo {
class RWorker;
template <typename Result>
class RPromise;
template <typename Result>
class RTask {
 public:
  using promise_type = RPromise<Result>;
  RTask(std::coroutine_handle<RTask<Result>::promise_type> handler,
        RWorker* worker)
      : handler(handler), worker(worker) {}
  void await_suspend(std::coroutine_handle<RPromise<Result>> h) {
    // the current coroutine is suspended, then call the worker to re-schedule
    this->worker->appendWork(h);  // impossible to return false
    return;  // then the control will be passed to the resumer
  }
  RWorker* worker;
  std::promise<Result> promise;
  // void await_suspend(std::coroutine_handle<RPromise<Result>> h);
  std::coroutine_handle<RTask<Result>::promise_type> handler;
};
template <typename Result>
class RPromise {
 public:
  RTask<Result> get_return_object() {
    return RTask<Result>(
        std::coroutine_handle<RPromise<Result>>::from_promise(*this), nullptr);
  }
  std::suspend_never initial_suspend() {
    return {};
  }  // never suspend initallly

  template <typename T>
    requires std::convertible_to<T, Result>
  std::suspend_always yield_value(T&& value) {
    // value_ = std::forward<T>(value);  // caching the result in promise
    return {};
  }
  std::suspend_never final_suspend() noexcept { return {}; }
  void unhandled_exception() noexcept {
    auto exception = std::current_exception();
  }
  template <typename T>
    requires std::convertible_to<T, Result>
  void return_value(T&& value) noexcept {
    return;
  }
};
}  // namespace RCo