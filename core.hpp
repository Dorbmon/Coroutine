#pragma once
#include <atomic>
#include "list.hpp"
#include "worker.hpp"
namespace RCo {
class Core {
 private:
  long coreID;
  std::atomic_long workerNum;
  RList<RWorker> workers;

 public:
  Core() = delete;
  Core(long coreID) { this->coreID = coreID; }
};
}  // namespace RCo