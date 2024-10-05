#include "task.h"
#include <coroutine>
#include <iostream>
#include "worker.h"

namespace RCo {
    // template <typename Result, template<typename> class OP_T>
    // template<typename IN_T>
    // void RTask<Result, OP_T>::await_suspend(std::coroutine_handle<RPromise<IN_T>> h) {
    //     OP_T<IN_T>::await_suspend(h, this->worker);
    // }

    // template <typename Result>
    // void RAlwaysJoin<Result>::await_suspend(std::coroutine_handle<RPromise<Result>> h, RWorker *worker) {
    //     std::cout << "Here" << std::endl;
    //     worker->appendWork(h);
    // }
    
} // namespace RCo