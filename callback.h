#pragma once

#include "task.h"
#include "utils.h"
#include "worker.h"
namespace RCo {
template <typename Result> class CallbackOP {
public:
  template <typename HANDLE_T>
  static void await_suspend(std::coroutine_handle<HANDLE_T> h,
                            RWorker *worker, void *metadata, std::shared_ptr<std::promise<Result>> promise) {
    // Correctly cast the metadata to the appropriate function type
    auto f = static_cast<
        std::function<void(std::unique_ptr<std::function<void(Result &&)>>)> *>(
        metadata);

    // Capture the function, coroutine handle, and worker in a lambda
    // Ensure that the function pointer is deleted after use to prevent memory
    // leaks
    auto callback = std::make_unique<std::function<void(Result &&)>>(
        [f, h, worker, promise](Result &&v) {
          promise->set_value(v);
          worker->appendWork(new dumyWorkBase([h]() { h.resume(); }));
        });
    (*f)(std::move(callback));
  }
};
template <> class CallbackOP<void> {
public:
  template <typename HANDLE_T>
  static void await_suspend(std::coroutine_handle<HANDLE_T> h,
                            RWorker *worker, void *metadata, std::shared_ptr<std::promise<void>> promise) {
    // Correctly cast the metadata to the appropriate function type
    auto f = static_cast<
        std::function<void(std::unique_ptr<std::function<void(void)>>)> *>(
        metadata);

    // Capture the function, coroutine handle, and worker in a lambda
    // Ensure that the function pointer is deleted after use to prevent memory
    // leaks
    auto callback =
        std::make_unique<std::function<void(void)>>([f, h, worker, promise]() {
          promise->set_value();
          worker->appendWork(new dumyWorkBase([h]() { h.resume(); }));
        });
    (*f)(std::move(callback));
  }
};
template <typename Ret_V, typename Function>
  requires FunctionWithCallback<Function, Ret_V>
static RTask<Ret_V, CallbackOP> callback_to_awaitable(Function &&f) {
  auto promise = (typename RTask<Ret_V, CallbackOP>::promise_type){};
  auto f_p = new std::function<void(std::function<void(Ret_V)> *)>(f);
  auto ret = promise.get_return_object();
  ret.metadata = (void *)f_p;
  return ret;
}

using void_callback_function = std::function<void(std::function<void()> *)>;
static RTask<void, CallbackOP>
callback_to_awaitable(void_callback_function &f) {
  auto promise = (typename RTask<void, CallbackOP>::promise_type){};
  auto f_p = new std::function<void(std::function<void(void)> *)>(f);
  auto ret = promise.get_return_object();
  ret.metadata = (void *)f_p;
  return ret;
}
using void_callback_function = std::function<void(std::function<void()> *)>;
static RTask<void, CallbackOP>
callback_to_awaitable(void_callback_function &&f) {
  auto promise = (typename RTask<void, CallbackOP>::promise_type){};
  auto f_p = new std::function<void(std::function<void(void)> *)>(f);
  auto ret = promise.get_return_object();
  ret.metadata = (void *)f_p;
  return ret;
}
} // namespace RCo