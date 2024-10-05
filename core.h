#pragma once
#include "worker.h"
#include <sched.h>
#include <vector>
namespace RCo {
class RScheduler;

class Core {
private:
    long                   coreID;
    std::mutex             workersLock;
    std::vector<RWorker *> workers;
    std::vector<RWorker *> monopolyWorkers;
public:
    RScheduler *scheduler;
    Core() = delete;
    Core(long coreID, RScheduler *scheduler);
    cpu_set_t toBindMask() {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(this->coreID, &mask);
        return mask;
    }
    RWorker *getOneWorker() noexcept;
    RWorker *createMonopolyWorker() noexcept;
    void     removeWorker(RWorker *worker) noexcept;
};


} // namespace RCo