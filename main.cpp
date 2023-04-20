#include <math.h>
#include <iostream>
#include "scheduler.h"
#include "task.h"
RCo::RTask<double> add(int a, int b) {
  double ret = 0;
  for (int i = 0; i < 10000; i++) {
    ret += a * b * i * log(a * i);
  }
  std::cout << "here" << std::endl;
  co_return ret;
}
int main() {
  auto ret = RCo::run(add, 1, 2);
  std::cout << "gogogo" << std::endl;
  auto v = ret.get();
  std::cout << "cal v:" << v << std::endl;
}