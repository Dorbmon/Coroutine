#include "core.h"
#include "scheduler.h"
#include "worker.h"
#include <algorithm>
#include <cstdlib>
#include <mutex>
namespace RCo {
Core::Core(long coreID, RScheduler *scheduler)
    : coreID(coreID)
    , scheduler(scheduler) {
    workers.reserve(scheduler->config.MaxSystemThreadPerCore);
}
RWorker *Core::createMonopolyWorker() noexcept {
    auto worker = new RWorker(this, false, RWORKER_CONFIG_MONOPOLY); // if it is NO.0 worker, it never dies
    worker->tagNewWorkIsOnTheWay();
    // workers.push_back(worker);
    monopolyWorkers.push_back(worker);
    return worker;
}
RWorker *Core::getOneWorker() noexcept {
    std::lock_guard<std::mutex> workersLockGuard(this->workersLock);
    // find a available free worker
    for (auto worker : workers) {
        if (!worker->isBusy()) {
            // tell it the new work is on the way and not to shutdown.
            if (!worker->tagNewWorkIsOnTheWay()) { // has start to shutdown.
                continue;
            }
            // then this worker is ok!
            return worker;
        }
    }
    // no free worker is available
    auto workerNum = workers.size();
    if (workerNum < scheduler->config.MaxSystemThreadPerCore) {
        // can add another worker
        auto worker = new RWorker(this, workerNum == 0); // if it is NO.0 worker, it never dies
        worker->tagNewWorkIsOnTheWay();
        workers.push_back(worker);
        return worker;
    }
    // TODO: better way to obtain a worker

    // DO it reversely so that we won't always select NO.0
    for (auto it = workers.rbegin(); it != workers.rend(); ++it) {
        auto worker = *it;
        // tell it the new work is on the way and not to shutdown.
        [[unlikely]] if (!worker->tagNewWorkIsOnTheWay()) { // has start to shutdown.
            continue;
        }
        // then this worker is ok!
        return worker;
    }
    // impossible to reach here as there is always a NO.0 worker available
    return nullptr;
}
void Core::removeWorker(RWorker *worker) noexcept {
    std::lock_guard<std::mutex> workersLockGuard(this->workersLock);
    auto pos = std::find(this->workers.begin(), this->workers.end(), worker);
    [[unlikely]] if (pos == this->workers.end()) {
        // try to find it in the monopoly workers
        pos = std::find(this->monopolyWorkers.begin(), this->monopolyWorkers.end(), worker);
        
        [[likely]] if (pos != this->monopolyWorkers.end()) {
            this->monopolyWorkers.erase(pos);
        }
    } else {
        this->workers.erase(pos);
    }
}
} // namespace RCo