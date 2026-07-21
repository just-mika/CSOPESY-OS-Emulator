#include "AScheduler.h"
#include "FlatMemoryAllocator.h"
#include <deque>
#include <thread>

SchedulingAlgorithm parseAlgorithm(std::string algo);

AScheduler::AScheduler(Config config)
    : OSThread(),
    algo(parseAlgorithm(config.scheduler)),
    numCPU(config.numCPU),
    quantumCycles(config.quantumCycles),
    batchProcessFreq(config.batchProcessFreq),
    minIns(config.minIns),
    maxIns(config.maxIns),
    maxOverallMem(config.maxOverallMem),
    memPerFrame(config.memPerFrame),
    memPerProc(config.memPerProc)
{ }
void AScheduler::checkMemoryBlockedQueue() {
    std::unique_lock lock(mutex);

    auto it = memoryQueue.begin();
    while (it != memoryQueue.end()) {
        auto process = *it;
        void* ptr = memoryAllocator->allocate(process->getMemoryRequired());
        if (ptr != nullptr) {
            process->setMemoryAddress(ptr);
            process->setState(ProcessState::READY);
            readyQueue.push_back(process);
            processTable[process->getPID()] = process;
            it = memoryQueue.erase(it);
        }
        else {
            break;
        }
    }
}

void AScheduler::addProcess(std::shared_ptr<Process> process) {
    // When a process comes, add them first to the memoryQueue
    // LOCK FIRST
    std::unique_lock lock(mutex);
    process->setState(ProcessState::WAITING);
    memoryQueue.push_back(process);
    lock.unlock();
    checkMemoryBlockedQueue();
}

std::shared_ptr<Process> AScheduler::findProcess(const std::string& processName) {
    std::shared_lock lock(mutex);
    for (int i = 0; i < processTable.size(); i++) {
        auto process = processTable[i];
        if (process && process->getName() == processName)
            return process;
    }
	lock.unlock();
    return nullptr;
}

std::shared_ptr<Process> AScheduler::findProcess(int pid) {
    std::shared_lock lock(mutex);
    auto it = processTable.find(pid);
    if (it != processTable.end()) {
        return it->second;
    }
    lock.unlock();
    return nullptr; 
}

void AScheduler::removeProcess(int pid) {
    processTable.erase(pid);
}

void AScheduler::run() {
    running = true;
    while (running) {
        this->sleep(100);
    }
}

void AScheduler::stop() {
    running = false;
}

SchedulingAlgorithm AScheduler::parseAlgorithm(std::string algo) {
    if (algo == "\"fcfs\"")
        return FCFS;
    else if (algo == "\"sjf-preemptive\"")
        return SJF_PREEMPTIVE;
    else if (algo == "\"sjf-nonpreemptive\"")
        return SJF_NONPREEMPTIVE;
    else if (algo == "\"rr\"")
        return RR;
    return FCFS;
}

std::deque<std::shared_ptr<Process>> AScheduler::getFinishedProcesses()
{
    return finishedProcesses;
}

std::deque<std::shared_ptr<Process>> AScheduler::getRunningProcesses()
{
    return runningProcesses;
}