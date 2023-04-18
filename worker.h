#pragma once
// each worker will be binded to their own cpu core (1 core to multi workers)
namespace RCo {
class RThread;
class Core;
class RWorker {
 public:
  Core* core;
  RThread* thread;
  RWorker() = delete;
  RWorker(Core* core);
  ~RWorker();

 private:
  void threadFunction() noexcept;
};
}  // namespace RCo
