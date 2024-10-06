#pragma once
#ifdef __APPLE__
#include "../../core.h"
#include <functional>
#include <mach/mach.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
namespace RCo {
inline long getCoreNum() {
  long ret = sysconf(_SC_NPROCESSORS_ONLN);
  if (ret == 0) {
    return 1;
  }
  return ret;
}

class RThread {
private:
  pthread_t threadHandler;
  std::function<void()> f;

  static void *thread_function(void *arg) noexcept {
    RThread *t = static_cast<RThread *>(arg);
    (t->f)();
    return nullptr;
  }

public:
  static pthread_t currentThreadID() { return pthread_self(); }

  Core *core;

  RThread(Core *core, std::function<void()> &&f) : f(f), core(core) {
    auto err = pthread_create(&threadHandler, NULL, &thread_function, this);
    if (err != 0) {
      throw std::runtime_error(strerror(err));
    }
    auto mach_thread_id = pthread_mach_thread_np(this->threadHandler);
    if (mach_thread_id == MACH_PORT_NULL) {
      throw std::runtime_error("pthread_mach_thread_np failed");
    }
    thread_affinity_policy_data_t policy;
    policy.affinity_tag = 1 << this->core->getCoreID();
    thread_policy_set(mach_thread_id, THREAD_AFFINITY_POLICY,
                      (thread_policy_t)&policy, THREAD_AFFINITY_POLICY_COUNT);
    // kern_return_t thread_policy_set(
    //     thread_t thread, thread_policy_flavor_t flavor,
    //     thread_policy_t policy_info, mach_msg_type_number_t count);
  }

  void Run() noexcept { pthread_detach(this->threadHandler); }

  ~RThread() { pthread_cancel(this->threadHandler); }
};
} // namespace RCo
#endif
