#include "task.h"
#include <coroutine>
#include "worker.h"

namespace RCo {
    template <typename Result>
    void RTask<Result>::await_suspend(std::coroutine_handle<RPromise<Result>> h) {
        // the current coroutine is suspended, then call the worker to re-schedule
        this->worker->appendWork(h); // impossible to return false
        return;                      // then the control will be passed to the resumer
    }
} // namespace RCo