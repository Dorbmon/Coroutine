#pragma once
#include "utils.h"

#include <concepts>
#include <coroutine>
#include <exception>
#include <functional>
#include <future>
#include <memory>
namespace RCo {

class RWorker;
template <typename Result>
class RPromise;

template <typename Result> 
class RTask {
public:
    using ResultType   = Result;
    using promise_type = RPromise<Result>;
    RTask(const RTask &task) {
        this->worker  = task.worker;
        this->handler = task.handler;
        this->promise = task.promise;
    }
    RTask(std::coroutine_handle<RTask<Result>::promise_type> handler, RWorker *worker,
          std::shared_ptr<std::promise<Result>> promise)
        : handler(handler)
        , worker(worker)
        , promise(promise) {
    }

    void await_suspend(std::coroutine_handle<RPromise<Result>>);
    RWorker                              *worker;
    std::shared_ptr<std::promise<Result>> promise;
    std::coroutine_handle<promise_type> handler;
};
template <typename Result> class RPromise {
private:
    std::shared_ptr<std::promise<Result>> promise = std::make_shared<std::promise<Result>>();
    RTask<Result>                         task;

public:
    RPromise()
        : task(std::coroutine_handle<RPromise<Result>>::from_promise(*this), nullptr, promise) {
    }
    RTask<Result> get_return_object() {
        return this->task;
    }
    std::suspend_always initial_suspend() {
        // so that we can have the worker to run current coroutine
        return {};
    } // waiting for the scheduler to resume it

    template <typename T>
        requires std::convertible_to<T, Result>
    std::suspend_always yield_value(T &&value) {
        // value_ = std::forward<T>(value);  // caching the result in promise
        return {};
    }
    std::suspend_never final_suspend() noexcept {
        return {};
    }
    void unhandled_exception() noexcept {
        auto exception = std::current_exception();
        // stop the panic here to avoid the crash of whole program
        // TODO: pass the crash to logger or user's custom handler
    }

    template <typename T>
        requires std::convertible_to<T, Result>
    void return_value(T &&value) noexcept {
        promise->set_value(value);
        return;
    }
};
template <typename F, typename... Args>
concept RTaskReturnable =
    requires(F &&f, Args &&...args) {
        is_specialization<typename decltype(std::function{f})::result_type, RTask>{};
    };
} // namespace RCo