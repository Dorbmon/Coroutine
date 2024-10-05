#pragma once
// each worker will be binded to their own cpu core (1 core to multi workers)

#include <atomic>
#include <list>
#include <mutex>
namespace RCo {
class RThread;
class Core;
class workBase {
public:
  virtual void resume() = 0;
};

template<typename F>
class dumyWorkBase: public workBase {
private:
  F f;
public:
  dumyWorkBase(F && f): f(f) {}
  virtual void resume() {
    this->f();
  }
};

#define rsched()                                                               \
  RCo::current_worker->appendWork(RCo::current_work);                          \
  co_await std::suspend_always {}

const int RWORKER_CONFIG_MONOPOLY = 1 << 1;
class RWorker {
public:
  int config = 0;
  Core *core;
  RThread *thread;
  RWorker() = delete;
  RWorker(RWorker&) = delete;
  RWorker(Core *core, bool neverDieWorker, int config = 0);
  ~RWorker();
  bool appendWork(workBase *work) noexcept;
  bool isBusy() noexcept;
  bool tagNewWorkIsOnTheWay() noexcept; // tell the worker not to shutdown as
                                        // the new work is coming on the way

private:
  bool neverDieWorker; // if this worker can never die
  std::atomic_bool shutdown = false;
  std::atomic_bool working = false;
  std::mutex worksLock;
  std::list<workBase *> works;
  std::atomic_uint32_t num_pending_task = {0};
  int newWorkTag = {0};
  void threadFunction() noexcept;
};
extern thread_local RWorker *current_worker;
} // namespace RCo
