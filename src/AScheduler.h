#pragma once
#include <string>
#include <memory>
#include <deque>
#include <shared_mutex>

#include "Process.h"
#include "OSThread.h"
#include "Config.h"
#include "PagedMemoryAllocator.h"

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
    static SchedulingAlgorithm parseAlgorithm(std::string algo);
    std::deque<std::shared_ptr<Process>> getFinishedProcesses();
    std::deque<std::shared_ptr<Process>> getRunningProcesses();
    std::shared_ptr<Process> findProcess(int pid);
    void checkMemoryBlockedQueue();
    void removeProcess(int pid);
    mutable std::shared_mutex mutex;
    

protected:
    SchedulingAlgorithm algo;
    int numCPU;
    unsigned long long quantumCycles;
    unsigned long long batchProcessFreq;
    unsigned long long minIns;
    unsigned long long maxIns;
    unsigned long long delaysPerExec;
    // Memory-related Configuration
    unsigned long long maxOverallMem;
    unsigned long long memPerFrame;
    unsigned long long minMemPerProc;
    unsigned long long maxMemPerProc;

    
    int cpuCycles = 0;
    int nextPID = 0;

    std::atomic<bool> running = false;
    std::deque<std::shared_ptr<Process>> readyQueue;
    std::deque<std::shared_ptr<Process>> finishedProcesses;
    std::deque<std::shared_ptr<Process>> runningProcesses;
    std::list<std::shared_ptr<Process>> sleepingProcesses;
    std::deque<std::shared_ptr<Process>> memoryQueue;
    std::unordered_map<int, std::shared_ptr<Process>> processTable;
    std::shared_ptr<PagedMemoryAllocator> memoryAllocator;
};