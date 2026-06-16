#pragma once
#include <string>
#include <memory>
#include <deque>

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
    std::shared_ptr<Process> findProcess(std::string processName);
    void run() override;
    void stop();
    virtual void init() = 0;
    static SchedulingAlgorithm parseAlgorithm(std::string algo);

protected:
    SchedulingAlgorithm algo;
    int numCPU;
    unsigned long long quantumCycles;
    unsigned long long batchProcessFreq;
    unsigned long long minIns;
    unsigned long long maxIns;
    unsigned long long delaysPerExec;

    bool running = false;
    std::deque<std::shared_ptr<Process>> readyQueue;
    std::deque<std::shared_ptr<Process>> finishedProcesses;
    std::deque<std::shared_ptr<Process>> runningProcesses;
};