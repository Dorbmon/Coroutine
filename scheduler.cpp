#include "scheduler.h"
#include <concepts>
#include <future>
#include "task.h"
#include "worker.h"
namespace RCo {
RScheduler* _defaultScheduler =
    new RScheduler(RSchedulerConfig{getCoreNum(), 2, 50});

template <typename... Args, typename returnValue, typename realValue>
  requires std::convertible_to<returnValue, RTask<realValue>>
std::promise<returnValue> RScheduler::run(std::function<returnValue(Args...)> f,
                                          const Args&... args) {
  // bind this task to one worker
  auto core = this->GetOneCore();
  auto worker = core->getOneWorker();
  RTask<realValue> awaitable = f(args...);
  worker->appendWork(work<realValue>{
      .task = awaitable});  // it has been tagged and will never return false
  return awaitable.promise;
}
}  // namespace RCo