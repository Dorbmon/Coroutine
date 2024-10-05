#include "scheduler.h"

#include "task.h"


namespace RCo {


template <typename returnValue> struct work : public workBase {
  RTask<returnValue> task;

  work(RTask<returnValue> task) : task(task) {}
  virtual void resume() { task.handler.resume(); }
};
RScheduler *_defaultScheduler = new RScheduler(RSchedulerConfig{getCoreNum(), 2, 50});

} // namespace RCo