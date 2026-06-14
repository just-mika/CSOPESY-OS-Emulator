#pragma once
#include <string>
#include <memory>

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

protected:
    SchedulingAlgorithm algo;
    int numCPU;
    unsigned long long quantumCycles;
    unsigned long long batchProcessFreq;
    unsigned long long minIns;
    unsigned long long maxIns;
    unsigned long long delaysPerExec;
};