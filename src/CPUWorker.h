#pragma once
#include <memory>
#include <mutex>

#include "OSThread.h"
#include "Process.h"

class CPUWorker : public OSThread
{
public:
    CPUWorker(int coreID);

    void assignProcess(std::shared_ptr<Process> process);
    std::shared_ptr<Process> getCurrentProcess() const;
    bool isFree() const;
    void update(bool isRunning);
    void run() override;
    int getCoreID();

private:
    int coreID;
    std::atomic<bool> running = false;
    std::shared_ptr<Process> currentProcess = nullptr;
    mutable std::mutex mutex;
    int cpuCycles = 0;
};