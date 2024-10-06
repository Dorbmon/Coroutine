#include "timer.h"
#include "callback.h"
#include "platform/time.h"
#include "scheduler.h"
#include <memory>
namespace RCo {

RTask<void> timer_main_loop();
int *year_wheel[10] = {nullptr};
__attribute__((constructor)) void init_timer() {
  // Start the timer worker. It is a heavy work
  RCo::run_heavy(timer_main_loop);
}
bool operator<(const TimerTask &a, const TimerTask &b) {
  return a.schedule_at < b.schedule_at;
}
std::mutex timer_lock;
std::priority_queue<std::unique_ptr<TimerTask>,
                    std::vector<std::unique_ptr<TimerTask>>,
                    TimerTaskComparator>
    task_heap;
unsigned long long sleep_interval_max = 50; // max ms for sleeping
RTask<void> timer_main_loop() {
  while (true) {
    // get the latest task
    std::unique_ptr<TimerTask> task;
    {
      std::lock_guard<std::mutex> timer_lock_guard(timer_lock);
      if (!task_heap.empty()) {
        // TODO: make the code more elegant
        task = std::move(
            const_cast<std::unique_ptr<TimerTask> &>(task_heap.top()));
        task_heap.pop();
      }
    }
    auto sleep_time = sleep_interval_max;
    if (task) {
      auto now = getCurrentMillisecs();
      if (task->schedule_at < now + sleep_interval_max) {
        // should run now
        task->f();
        continue;
      }
      sleep_time = std::min(task->schedule_at - now, sleep_time);
    }
    usleep(sleep_time * 1000);
  }
  co_return;
}

RTask<void> sleep(unsigned long long delay_ms) {
  std::function<void(std::unique_ptr<std::function<void()>>)> f =
      [](std::unique_ptr<std::function<void(void)>>) {};

  co_await callback_to_awaitable(std::move(f));
}
} // namespace RCo