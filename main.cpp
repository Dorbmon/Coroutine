#include <math.h>
#include <iostream>
#include "scheduler.h"
#include "task.h"
#include "utils.h"
#include "worker.h"
RCo::RTask<long long> add(int a, int b) {
  long long ret = 0;
  for (int i = 0; i < 10000; i++) {
    ret += a * b * i;
  }
  std::cout << "here" << std::endl;
  // auto worker = __GetWorker();
  auto worker = RCo::current_worker;
  std::cout << "busy:" << worker->isBusy() << std::endl;
  co_return ret;
}
int main() {
  auto ret = RCo::run(add, 1, 2);
  std::cout << "gogogo" << std::endl;
  for (int i = 0; i < 100000; i++) {
  }
  std::cout << "rfnish" << std::endl;
  auto v = ret.get();
  std::cout << "cal v:" << v << std::endl;
}