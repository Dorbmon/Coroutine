#include "worker.h"
#include <functional>
#include "thread.h"
namespace RCo {
RWorker::RWorker(Core* core) : core(core) {
  this->thread = new RThread(core, std::bind(&RWorker::threadFunction, this));
}
}  // namespace RCo