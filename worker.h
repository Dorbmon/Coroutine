#pragma once
// each worker will be binded to their own cpu core (1 core to multi workers)
#include "task.h"
#include <atomic>
#include <list>
#include <mutex>
namespace RCo {
class RThread;
class Core;
class workBase {
public:
    virtual void resume() = 0;
};

#define rsched()                                        \
    RCo::current_worker->appendWork(RCo::current_work); \
    co_await std::suspend_always {}

template <typename returnValue> struct work : public workBase {
    RTask<returnValue> task;

    work(RTask<returnValue> task)
        : task(task) {
    }
    virtual void resume() {
        task.handler.resume();
    }
};
class RWorker {
public:
    Core    *core;
    RThread *thread;
    RWorker() = delete;
    RWorker(Core *core, bool neverDieWorker);
    ~RWorker();
    bool appendWork(workBase *work) noexcept;
    bool isBusy() noexcept;
    bool tagNewWorkIsOnTheWay() noexcept; // tell the worker not to shutdown as
                                          // the new work is coming on the way

private:
    bool                  neverDieWorker; // if this worker can never die
    std::atomic_bool      shutdown = false;
    std::atomic_bool      working  = false;
    std::mutex            worksLock;
    std::list<workBase *> works;
    std::atomic_int       newWorkTag = 0;
    void                  threadFunction() noexcept;
};
extern thread_local RWorker  *current_worker;
} // namespace RCo
