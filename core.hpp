#pragma once
#include <sched.h>
#include <atomic>
#include "list.hpp"
#include "worker.h"
namespace RCo {
class RScheduler;
class Core {
 private:
  long coreID;
  std::atomic_long workerNum;
  RList<RWorker> workers;
  RScheduler* scheduler;

 public:
  Core() = delete;
  Core(long coreID, RScheduler* scheduler)
      : scheduler(scheduler), coreID(coreID) {}
  cpu_set_t toBindMask() {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(this->coreID, &mask);
    return mask;
  }
};
}  // namespace RCo