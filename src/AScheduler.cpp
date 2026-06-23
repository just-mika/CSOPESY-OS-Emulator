#include "AScheduler.h"
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
    delaysPerExec(config.delaysPerExec)
{ }


void AScheduler::addProcess(std::shared_ptr<Process> process) {
    std::unique_lock lock(mutex);
    readyQueue.push_back(process);
    lock.unlock();
}

std::shared_ptr<Process> AScheduler::findProcess(const std::string& processName) {
    std::shared_lock lock(mutex);
    for (auto& process : readyQueue) {
        if (process->getName() == processName)
            return process;
    }
	lock.unlock();
    return nullptr;
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