#pragma once
#include "worker.h"
#include <sched.h>
#include <vector>
namespace RCo {
class RScheduler;

class Core {
private:
  long coreID;
  std::mutex workersLock;
  std::vector<RWorker *> workers;
  std::vector<RWorker *> monopolyWorkers;

public:
  RScheduler *scheduler;
  Core() = delete;
  Core(long coreID, RScheduler *scheduler);
  RWorker *getOneWorker() noexcept;
  RWorker *createMonopolyWorker() noexcept;
  void removeWorker(RWorker *worker) noexcept;
  long getCoreID() { return this->coreID; }
};

} // namespace RCo