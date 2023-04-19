#include "task.h"
#include "worker.h"

namespace RCo {
template <typename Result>
RTask<Result>::RTask(std::coroutine_handle<RTask<Result>::promise_type> handler,
                     RWorker* worker)
    : handler(handler), worker(worker) {}
template <typename Result>
void RTask<Result>::await_suspend(std::coroutine_handle<RPromise<Result>> h) {
  // the current coroutine is suspended, then call the worker to re-schedule
  this->worker->appendWork(h);  // impossible to return false
}
}  // namespace RCo