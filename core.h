#pragma once
#include <sched.h>
#include <atomic>
#include <vector>
#include "list.hpp"
#include "worker.h"
namespace RCo {
class RScheduler;
class Core {
 private:
  long coreID;
  std::atomic_long workerNum;
  std::mutex workersLock;
  std::vector<RWorker*> workers;
  RScheduler* scheduler;

 public:
  Core() = delete;
  Core(long coreID, RScheduler* scheduler);
  cpu_set_t toBindMask() {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(this->coreID, &mask);
    return mask;
  }
  RWorker* getOneWorker() noexcept;
};
}  // namespace RCo