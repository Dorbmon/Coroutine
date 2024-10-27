#pragma once
#ifdef __linux
#include <time.h>
namespace RCo {
static unsigned long long getCurrentMillisecs() {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME_COARSE, &ts);
  return ts.tv_sec * 1000 + ts.tv_nsec / (1000 * 1000);
}
} // namespace RCo

#endif