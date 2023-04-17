#pragma once
#include <coroutine>
namespace RCo {
struct RSchedulerConfig {
  int MaxCores;
  int MaxSystemThread;
};
class RScheduler {
 private:
  RSchedulerConfig config;

 public:
  RScheduler() = delete;
  RScheduler(RSchedulerConfig config) { this->config = config; }
};
}  // namespace RCo