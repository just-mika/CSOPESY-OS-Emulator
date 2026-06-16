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
    readyQueue.push_back(process);
}

std::shared_ptr<Process> AScheduler::findProcess(std::string processName) {
    for (auto& process : readyQueue) {
        if (process->getName() == processName)
            return process;
    }
    return nullptr;
}

void AScheduler::run() {
    running = true;
    while (running) {
        OSThread::sleep(100);
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
