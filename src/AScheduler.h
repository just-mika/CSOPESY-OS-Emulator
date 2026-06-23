#pragma once
#include <string>
#include <memory>
#include <deque>
#include <shared_mutex>

#include "Process.h"
#include "OSThread.h"
#include "Config.h"

enum SchedulingAlgorithm
{
    FCFS,
    SJF_PREEMPTIVE,
    SJF_NONPREEMPTIVE,
    RR
};

class AScheduler : public OSThread
{
public:
    AScheduler(Config config);

    void addProcess(std::shared_ptr<Process> process);
    std::shared_ptr<Process> findProcess(const std::string& processName);
    void run() override;
    void stop();
    virtual void init() = 0;
    static SchedulingAlgorithm parseAlgorithm(std::string algo);
    std::deque<std::shared_ptr<Process>> getFinishedProcesses();
    std::deque<std::shared_ptr<Process>> getRunningProcesses();

protected:
    SchedulingAlgorithm algo;
    int numCPU;
    unsigned long long quantumCycles;
    unsigned long long batchProcessFreq;
    unsigned long long minIns;
    unsigned long long maxIns;
    unsigned long long delaysPerExec;

    mutable std::shared_mutex mutex;
    int cpuCycles = 0;
    int nextPID = 0;

    std::atomic<bool> running = false;
    std::deque<std::shared_ptr<Process>> readyQueue;
    std::deque<std::shared_ptr<Process>> finishedProcesses;
    std::deque<std::shared_ptr<Process>> runningProcesses;
};