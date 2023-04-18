
#ifdef __linux
#include <exception>
#include <functional>

#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include "core.h"
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
    (t->f)();
    return nullptr;
  }

 public:
  Core* core;
  RThread(Core* core, std::function<void()>&& f) : f(f), core(core) {
    auto err = pthread_create(&threadHandler, NULL, &thread_function, this);
    if (err != 0) {
      throw strerror(err);
    }
    auto mask = this->core->toBindMask();
    pthread_setaffinity_np(this->threadHandler, sizeof(mask), &mask);
  }
  void Run() noexcept { pthread_detach(this->threadHandler); }
  ~RThread() { pthread_cancel(this->threadHandler); }  // not immediately
};
}  // namespace RCo
#endif