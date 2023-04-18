#include "core.h"
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
  if (workers.size() < scheduler->config.MaxSystemThreadPerCore) {
    // can add another worker
    auto worker = new RWorker(this);
    workers.push_back(worker);
    return worker;
  }
  // TODO: better way to obtain a worker
  return workers[rand() % workers.size()];
}
}  // namespace RCo