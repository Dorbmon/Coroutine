#include "task.h"
#include "worker.h"

namespace RCo {
template <typename Result>
RTask<Result>::RTask(std::coroutine_handle<RTask<Result>::promise_type> handler,
                     Core* core)
    : handler(handler), core(core) {}
template <typename Result>
void RTask<Result>::await_suspend(std::coroutine_handle<> h) {
  // the current coroutine is suspended, then pass the control to the scheduler
  // get the core scheduler to obtain one another task to switch to
}
}  // namespace RCo