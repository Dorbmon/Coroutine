#pragma once
#include <coroutine>
#include <list>
#include <stdexcept>
#include "core.h"
#include "thread.h"
namespace RCo {
struct RSchedulerConfig {
  long MaxCores;
  long MaxSystemThreadPerCore;
  long MaxNoTaskTryTime;
};
class RScheduler {
 private:
  long useCoreNum;
  std::vector<Core*> cores;
  unsigned long long coreRange = 0;

 public:
  RSchedulerConfig config;
  RScheduler() = delete;
  RScheduler(RSchedulerConfig config) {
    [[unlikely]] if (config.MaxCores <= 0) {
      throw std::invalid_argument("Invalid config");
    }
    this->config = config;
    useCoreNum = std::min(getCoreNum(), config.MaxCores);
    for (long i = 0; i < useCoreNum; i++) {
      this->cores.push_back(new Core(i, this));
    }
  }
  Core* GetOneCore() noexcept { return cores[(coreRange++) % cores.size()]; }
};
extern RScheduler* _defaultScheduler;
}  // namespace RCo