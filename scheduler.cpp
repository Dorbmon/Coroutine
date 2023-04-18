#include "scheduler.h"
namespace RCo {
RScheduler* _defaultScheduler =
    new RScheduler(RSchedulerConfig{getCoreNum(), 2});
}  // namespace RCo