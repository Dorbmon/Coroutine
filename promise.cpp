#include "scheduler.h"
#include "task.h"
namespace RCo {
template <typename Result>
RTask<Result> RPromise<Result>::get_return_object() {
  auto core = _defaultScheduler->GetOneCore();
  // distribute one worker for this target
  // TODO: better algorithm to distribte the worker
  // core->
  return RTask<Result>(
      std::coroutine_handle<RPromise<Result>>::from_promise(*this), core);
}
}  // namespace RCo