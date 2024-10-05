#include "worker.h"
#include "scheduler.h"
#include "thread.h"
#include <functional>
#include <mutex>
#include <pthread.h>
namespace RCo {
thread_local RWorker  *current_worker;
thread_local workBase *current_work;
RWorker::RWorker(Core *core, bool neverDieWorker, int config)
    : core(core)
    , neverDieWorker(neverDieWorker)
    , config(config) {
    this->thread = new RThread(core, std::bind(&RWorker::threadFunction, this));
}
RWorker::~RWorker() {
    delete this->thread;
}
bool RWorker::appendWork(workBase *work) noexcept {
    std::lock_guard worksLockGuard(this->worksLock);
    [[unlikely]] if (shutdown) {
        return false;
    }
    newWorkTag--;
    this->works.push_back(work);
    return true;
}
bool RWorker::isBusy() noexcept {
    if (this->shutdown) {
        return true;
    }
    return this->working;
}
bool RWorker::tagNewWorkIsOnTheWay() noexcept {
    std::lock_guard worksLockGuard(this->worksLock);
    [[unlikely]] if (this->shutdown) {
        return false;
    }
    newWorkTag++;
    return true;
}
void RWorker::threadFunction() noexcept {
    current_worker    = this;
    int noTaskTryTime = 0;
    while (true) {
        // TODO: better schedule and schedule between thread
        // pick a work to resume
        this->worksLock.lock();
        if (!this->works.empty()) {
            noTaskTryTime = 0;
            auto work     = this->works.front();
            this->works.pop_front();
            this->worksLock.unlock();
            this->working = true;
            current_work  = work;
            work->resume();
        } else {
            this->worksLock.unlock();
            this->working = false;
            noTaskTryTime++;
            if (num_pending_task == 0 && noTaskTryTime > this->core->scheduler->config.MaxNoTaskTryTime && !neverDieWorker) {
                // then this thread should stop to release system resource
                // refuse to add any work to this worker
                this->worksLock.lock();
                if (!this->works.empty() ||
                    this->newWorkTag > 0) { // new work or new work on the way, then stop shutdown
                    this->worksLock.unlock();
                    continue;
                }
                // starts to shutdown
                shutdown = true;
                this->worksLock.unlock();
                this->core->removeWorker(this);
                pthread_exit(nullptr); // this worker no longer exists
                return;
            }
        }
        sched_yield(); // pass the cpu to other workers(threads) on this core
    }
}
} // namespace RCo