#pragma once
#include <coroutine>
#include <list>
#include <stdexcept>
#include "core.hpp"
#include "thread.h"
namespace RCo {
struct RSchedulerConfig {
  long MaxCores;
  long MaxSystemThread;
};
class RScheduler {
 private:
  RSchedulerConfig config;
  long useCoreNum;
  std::list<Core> cores;

 public:
  RScheduler() = delete;
  RScheduler(RSchedulerConfig config) {
    [[unlikely]] if (config.MaxCores <= 0) {
      throw std::invalid_argument("Invalid config");
    }
    this->config = config;
    useCoreNum = std::min(getCoreNum(), config.MaxCores);
    for (long i = 0; i < useCoreNum; i++) {
      this->cores.push_back(Core(i, this));
    }
  }
};
}  // namespace RCo