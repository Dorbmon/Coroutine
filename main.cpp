#include "scheduler.h"
#include "task.h"
#include "worker.h"
#include <iostream>
#include <math.h>
#include "callback.h"

RCo::RTask<void> test_task() {
    co_return ;
}
void callback_f(std::unique_ptr<std::function<void(int&&)>> f) {
    std::cout << "Start callback" << std::endl;
    (*f)(1);
}
RCo::RTask<long long> add(int a, int b) {
    co_await RCo::callback_to_awaitable<int>(callback_f);
    std::cout << "Go" << std::endl;
    co_await test_task();
    
    long long ret = 0;
    for (int i = 0; i < 10000; i++) {
        ret += a * b * i;
        sleep(1);
        std::cout << "Count:" << ret << std::endl;
        // rsched();
    }
    // auto worker = __GetWorker();
    auto worker = RCo::current_worker;
    std::cout << "busy:" << worker->isBusy() << std::endl;
    co_return ret;
}
int main() {
    auto ret = RCo::run(add, 1, 2);
    // auto ret2 = RCo::run(add, 2, 3);
    std::cout << "gogogo" << std::endl;
    for (int i = 0; i < 100000; i++) {
    }
    sleep(10);
    std::cout << "rfnish" << std::endl;
    auto v = ret.get();
    std::cout << "cal v:" << v << std::endl;
}