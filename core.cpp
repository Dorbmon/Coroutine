#include "core.h"
#include <algorithm>
#include <cstdlib>
#include "scheduler.h"
#include "worker.h"
namespace RCo {
Core::Core(long coreID, RScheduler* scheduler)
    : coreID(coreID), scheduler(scheduler) {
  workers.reserve(scheduler->config.MaxSystemThreadPerCore);
}
RWorker* Core::getOneWorker() noexcept {
  workersLock.lock();
  // find a available free worker
  for (auto worker : workers) {
    if (!worker->isBusy()) {
      // tell it the new work is on the way and not to shutdown.
      if (!worker->tagNewWorkIsOnTheWay()) {  // has start to shutdown.
        continue;
      }
      // then this worker is ok!
      workersLock.unlock();
      return worker;
    }
  }
  // no free worker is available
  auto workerNum = workers.size();
  if (workerNum < scheduler->config.MaxSystemThreadPerCore) {
    // can add another worker
    auto worker = new RWorker(
        this, workerNum == 0);  // if it is NO.0 worker, it never dies
    worker->tagNewWorkIsOnTheWay();
    workers.push_back(worker);
    workersLock.unlock();
    return worker;
  }
  // TODO: better way to obtain a worker
  for (auto worker : workers) {
    // tell it the new work is on the way and not to shutdown.
    if (!worker->tagNewWorkIsOnTheWay()) {  // has start to shutdown.
      continue;
    }
    // then this worker is ok!
    workersLock.unlock();
    return worker;
  }
  // impossible to reach here as there is always a NO.0 worker available
  workersLock.unlock();
  return nullptr;
}
void Core::removeWorker(RWorker* worker) noexcept {
  this->workersLock.lock();
  auto pos = std::find(this->workers.begin(), this->workers.end(), worker);
  this->workers.erase(pos);
  this->workersLock.unlock();
}
}  // namespace RCo