#pragma once
#include <sched.h>
#include <atomic>
#include <vector>
#include "worker.h"
namespace RCo {
class RScheduler;
class Core {
 private:
  long coreID;
  std::mutex workersLock;
  std::vector<RWorker*> workers;

 public:
  RScheduler* scheduler;
  Core() = delete;
  Core(long coreID, RScheduler* scheduler);
  cpu_set_t toBindMask() {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(this->coreID, &mask);
    return mask;
  }
  RWorker* getOneWorker() noexcept;
  void removeWorker(RWorker* worker) noexcept;
};
}  // namespace RCo