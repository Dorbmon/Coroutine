#include "worker.h"
#include <pthread.h>
#include <functional>
#include "scheduler.h"
#include "thread.h"
namespace RCo {
RWorker::RWorker(Core* core, bool neverDieWorker)
    : core(core), neverDieWorker(neverDieWorker) {
  this->thread = new RThread(core, std::bind(&RWorker::threadFunction, this));
}
RWorker::~RWorker() {
  delete this->thread;
}
bool RWorker::appendWork(workBase work) noexcept {
  this->worksLock.lock();
  [[unlikely]] if (shutdown) {
    this->worksLock.lock();
    return false;
  }
  newWorkTag--;
  this->works.push_back(work);
  this->worksLock.unlock();
  return true;
}
bool RWorker::isBusy() noexcept {
  if (this->shutdown) {
    return true;
  }
  return this->working;
}
bool RWorker::tagNewWorkIsOnTheWay() noexcept {
  worksLock.lock();
  [[unlikely]] if (this->shutdown) {
    worksLock.unlock();
    return false;
  }
  newWorkTag++;
  worksLock.unlock();
  return true;
}
void RWorker::threadFunction() noexcept {
  int noTaskTryTime = 0;
  while (true) {
    // TODO: better schedule and schedule between thread
    // pick a work to resume
    this->worksLock.lock();
    if (!this->works.empty()) {
      noTaskTryTime = 0;
      auto work = this->works.front();
      this->works.pop_front();
      this->worksLock.unlock();
      this->working = true;
      work.resume();
    } else {
      this->worksLock.unlock();
      this->working = false;
      noTaskTryTime++;
      if (noTaskTryTime > this->core->scheduler->config.MaxNoTaskTryTime &&
          !neverDieWorker) {
        // then this thread should stop to release system resource
        // refuse to add any work to this worker
        this->worksLock.lock();
        if (!this->works.empty() ||
            this->newWorkTag >
                0) {  // new work or new work on the way, then stop shutdown
          this->worksLock.unlock();
          continue;
        }
        // starts to shutdown
        shutdown = true;
        this->worksLock.unlock();
        pthread_exit(nullptr);  // this worker no longer exists
        this->core->removeWorker(this);
        return;
      }
    }
    sched_yield();  // pass the cpu to other workers
  }
}
}  // namespace RCo