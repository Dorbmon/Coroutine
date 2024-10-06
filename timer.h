#pragma once
#include "platform/darwin/time.h"
#include "scheduler.h"
#include <memory>
#include <mutex>
#include <queue>

namespace RCo {

struct TimerTask {
  unsigned long long schedule_at;
  std::function<void()> f;
};
struct TimerTaskComparator {
  bool operator()(const std::unique_ptr<TimerTask> &lhs,
                  const std::unique_ptr<TimerTask> &rhs) const {
    return (*lhs).schedule_at > (*rhs).schedule_at;
  }
};

extern std::mutex timer_lock;
extern std::priority_queue<std::unique_ptr<TimerTask>,
                           std::vector<std::unique_ptr<TimerTask>>,
                           TimerTaskComparator>
    task_heap;

template <typename F, typename... Args>
  requires RTaskReturnable<F, Args...>
static void delay(unsigned long long delay_ms, F f, const Args &...args) {
  auto current_time = getCurrentMillisecs();

  // To erase the template
  auto task = std::make_unique<TimerTask>(current_time + delay_ms,
                                          [f, args...]() { run(f, args...); });
  std::lock_guard<std::mutex> timer_lock_guard(timer_lock);
  task_heap.push(std::move(task));
}

RTask<void> sleep(unsigned long long delay_ms);
} // namespace RCo