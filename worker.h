#pragma once
// each worker will be binded to their own cpu core (1 core to multi workers)
#include <atomic>
#include <coroutine>
#include <future>
#include <list>
#include <mutex>
#include "task.h"
namespace RCo {
class RThread;
class Core;
struct workBase {
  virtual void resume();
};
template <typename returnValue>
struct work : public workBase {
  RTask<returnValue> task;
  void resume() { task.resume(); }
};
class RWorker {
 public:
  Core* core;
  RThread* thread;
  RWorker() = delete;
  RWorker(Core* core, bool neverDieWorker);
  ~RWorker();
  bool appendWork(workBase work) noexcept;
  bool isBusy() noexcept;
  bool tagNewWorkIsOnTheWay() noexcept;  // tell the worker not to shutdown as
                                         // the new work is coming on the way

 private:
  bool neverDieWorker;  // if this worker can never die
  std::atomic_bool shutdown = false;
  std::atomic_bool working = false;
  std::mutex worksLock;
  std::list<workBase> works;
  std::atomic_int newWorkTag = 0;
  void threadFunction() noexcept;
};
}  // namespace RCo
