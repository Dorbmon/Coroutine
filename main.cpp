#include "scheduler.h"
#include "task.h"

RCo::RTask<int> add(int a, int b) {
  co_return a + b;
}
int main() {
  RCo::run(add, 1, 2);
}