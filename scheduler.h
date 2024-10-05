#pragma once
#include "core.h"
#include "thread.h"
#include <future>
#include <stdexcept>
namespace RCo {

template <typename F, typename... Args>
concept RTaskReturnable = requires(F &&f, Args &&...args) {
  // is_specialization<typename decltype(std::function{f})::result_type, RTask>{};
  std::true_type{};
};

struct RSchedulerConfig {
    long MaxCores;
    long MaxSystemThreadPerCore;
    long MaxNoTaskTryTime;
};

class RScheduler {
private:
    long                useCoreNum;
    std::vector<Core *> cores;
    unsigned long long  coreRange = 0;

public:
    RSchedulerConfig config;
    RScheduler() = delete;
    RScheduler(RSchedulerConfig config) {
        [[unlikely]] if (config.MaxCores <= 0) { throw std::invalid_argument("Invalid config"); }
        this->config = config;
        useCoreNum   = std::min(getCoreNum(), config.MaxCores);
        for (long i = 0; i < useCoreNum; i++) {
            this->cores.push_back(new Core(i, this));
        }
    }
    Core *GetOneCore() noexcept {
        return cores[ (coreRange++) % cores.size() ];
    }
    template <typename F, typename... Args>
        requires RTaskReturnable<F, Args...>
    auto run(F f, const Args &...args) -> std::future<typename decltype(f(args...))::ResultType> {
        using return_type     = decltype(f(args...));
        auto        core      = this->GetOneCore();
        auto        worker    = core->getOneWorker();
        return_type awaitable = f(args...);
        awaitable.worker      = worker; // set the worker
        worker->appendWork(new dumyWorkBase{
            [handler = awaitable.handler]() { handler.resume(); }}); // it has been tagged and will never return false
        return awaitable.promise->get_future();
        // F::
    }

    template <typename F, typename... Args>
        requires RTaskReturnable<F, Args...>
    auto run_heavy(F f, const Args &...args) -> std::future<typename decltype(f(args...))::ResultType> {
        using return_type     = decltype(f(args...));
        auto        core      = this->GetOneCore();
        auto        worker    = core->createMonopolyWorker();
        return_type awaitable = f(args...);
        awaitable.worker      = worker; // set the worker
        worker->appendWork(new dumyWorkBase{
            [handler = awaitable.handler]() { handler.resume(); }}); // it has been tagged and will never return false
        return awaitable.promise->get_future();
    }
    // template <typename F, typename... Args>
    // // requires std::convertible_to<F, std::function<decltype(F())(Args...)>>
    // auto run(F f, const Args&... args) -> std::promise<decltype(F())> {
    //   // bind this task to one worker
    //   using return_v = typename decltype(std::function{f})::result_type;
    //   auto core = this->GetOneCore();
    //   auto worker = core->getOneWorker();
    //   if constexpr (is_specialization<return_v, RTask>{}) {
    //     // then no need to convert
    //     return_v awaitable = f(args...);
    //     awaitable.worker = worker;  // set the worker
    //     worker->appendWork(work<decltype(F())>{
    //         .task =
    //             awaitable});  // it has been tagged and will never return false
    //     return awaitable.promise;
    //   } else {
    //     RTask<return_v> awaitable = f(args...);
    //     awaitable.worker = worker;  // set the worker
    //     worker->appendWork(work<decltype(F())>{
    //         .task =
    //             awaitable});  // it has been tagged and will never return false
    //     return awaitable.promise;
    //   }
    //   // // RTask<return_v> awaitable = f(args...);
    //   // awaitable.worker = worker;  // set the worker
    //   // worker->appendWork(work<decltype(F())>{
    //   //     .task = awaitable});  // it has been tagged and will never return
    //   //     false
    //   // return awaitable.promise;
    // }
};
extern RScheduler *_defaultScheduler;
template <typename F, typename... Args>
    requires RTaskReturnable<F, Args...>
static auto run(F f, const Args &...args)
    -> std::future<typename decltype(f(args...))::ResultType> {
    return _defaultScheduler->run(f, args...);
}
} // namespace RCo