#include "worker.h"
#include <pthread.h>
#include <functional>
#include "thread.h"
namespace RCo {
RWorker::RWorker(Core* core) : core(core) {
  this->thread = new RThread(core, std::bind(&RWorker::threadFunction, this));
}
RWorker::~RWorker() {
  delete this->thread;
}
void RWorker::threadFunction() noexcept {
  while (true) {
    // get a work from the core work list
    // if (core->)
    sched_yield();  // pass the cpu to other workers
  }
}
}  // namespace RCo