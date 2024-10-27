#pragma once
#include "utils.h"

#include "worker.h"
#include <concepts>
#include <coroutine>
#include <exception>
#include <future>
#include <iostream>
#include <memory>
namespace RCo {

class RWorker;
template <typename Result, template <typename> class OP_T> class RPromise;

template <template <typename> class OP_T, typename Result>
concept OP_Type =
    requires(std::coroutine_handle<RPromise<Result, OP_T>> h, RWorker *worker) {
      OP_T<Result>::await_suspend(h, worker);
    };

template <typename Result> class RAlwaysJoinOP {
public:
  template <typename HANDLE_T>
  static void await_suspend(std::coroutine_handle<HANDLE_T> h,
                            RWorker *worker, void *metadata, std::shared_ptr<std::promise<Result>> promise) {
    worker->appendWork(new dumyWorkBase([h = h]() { h.resume(); }));
  }
};

template <typename Result, template <typename> class OP_T = RAlwaysJoinOP>

class RTask {
public:
  using ResultType = Result;
  template <typename OP_IN_T> using OPType = OP_T<OP_IN_T>;
  using promise_type = RPromise<Result, OP_T>;
  RTask(const RTask &task) {
    this->worker = task.worker;
    this->handler = task.handler;
    this->promise = task.promise;
  }
  bool await_ready() { return false; }
  
  Result await_resume() {
    // return this->promise.get();
    if constexpr(std::is_void_v<Result>) {
      return;
    } else {
      return this->promise->get_future().get();
    }
  }
  RTask(std::coroutine_handle<RTask<Result, OP_T>::promise_type> handler,
        RWorker *worker, std::shared_ptr<std::promise<Result>> promise)
      : handler(handler), worker(worker), promise(promise) {}

  template <typename HANDLE_T>

  void await_suspend(std::coroutine_handle<HANDLE_T> h) {
    OP_T<Result>::await_suspend(h, this->worker, metadata, promise);
  }
  RWorker *worker;
  std::shared_ptr<std::promise<Result>> promise;
  std::coroutine_handle<promise_type> handler;
  void *metadata = nullptr;
};

template <typename Result, template <typename> class OP_T>
class __RPromise_Base {
public:
  std::shared_ptr<std::promise<Result>> promise =
      std::make_shared<std::promise<Result>>();
  RTask<Result, OP_T> task;

public:
  __RPromise_Base(
      std::coroutine_handle<typename RTask<Result, OP_T>::promise_type> handler,
      RWorker *worker)
      : task(handler, worker, promise) {}
  RTask<Result, OP_T> get_return_object() { return this->task; }
  std::suspend_always initial_suspend() {
    // so that we can have the worker to run other coroutine
    return {};
  } // waiting for the scheduler to resume it

  template <typename T>
    requires std::convertible_to<T, Result>
  std::suspend_always yield_value(T &&value) {
    // value_ = std::forward<T>(value);  // caching the result in promise
    return {};
  }
  std::suspend_never final_suspend() noexcept { return {}; }
  void unhandled_exception() noexcept {
    auto exception = std::current_exception();
    // stop the panic here to avoid the crash of whole program
    // TODO: pass the crash to logger or user's custom handler
  }
};

template <typename Result, template <typename> class OP_T>
class RPromise : public __RPromise_Base<Result, OP_T> {
public:
  RPromise()
      : __RPromise_Base<Result, OP_T>(
            std::coroutine_handle<RPromise<Result, OP_T>>::from_promise(*this),
            current_worker) {}
  template <typename T, typename U = Result>
    requires std::convertible_to<T, Result>
  void return_value(T &&value) noexcept {
    this->promise->set_value(value);
  }
};
template <template <typename> class OP_T>
class RPromise<void, OP_T> : public __RPromise_Base<void, OP_T> {
public:
  RPromise()
      : __RPromise_Base<void, OP_T>(
            std::coroutine_handle<RPromise<void, OP_T>>::from_promise(*this),
            current_worker) {}

  void return_void() noexcept { this->promise->set_value(); }
};

} // namespace RCo