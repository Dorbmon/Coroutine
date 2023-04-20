#include "scheduler.h"
#include <concepts>
#include <future>

namespace RCo {
RScheduler *_defaultScheduler = new RScheduler(RSchedulerConfig{getCoreNum(), 2, 50});

} // namespace RCo