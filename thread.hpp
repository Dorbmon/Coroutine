#ifdef __linux
#include <pthread.h>
#include <unistd.h>
#include <thread>
inline long getCoreNum() {
  long ret = sysconf(_SC_NPROCESSORS_ONLN);
  [[unlikely]] if (ret == 0) { return 1; }
  return ret;
}
#endif