#include <exception>
#include <functional>

#ifdef __linux
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include "core.hpp"
namespace RCo {
inline long getCoreNum() {
  long ret = sysconf(_SC_NPROCESSORS_ONLN);
  [[unlikely]] if (ret == 0) { return 1; }
  return ret;
}

class RThread {
 private:
  pthread_t threadHandler;

  std::function<void()> f;
  static void* thread_function(void* arg) noexcept {
    RThread* t = static_cast<RThread*>(arg);
    auto mask = t->core->toBindMask();
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
      // failed
    }
    (t->f)();
    return nullptr;
  }

 public:
  Core* core;
  RThread(Core* core, std::function<void()> f) : f(f), core(core) {
    auto err = pthread_create(&threadHandler, NULL, &thread_function, this);
    if (err != 0) {
      throw strerror(err);
    }
  }
};
}  // namespace RCo
#endif