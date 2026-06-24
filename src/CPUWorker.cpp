#include "CPUWorker.h"
#include <iostream>
#include <thread>

#include "GlobalScheduler.h"


CPUWorker::CPUWorker(int coreID)
    : coreID(coreID)
{
    this->running = false;
    this->currentProcess = nullptr;
}

void CPUWorker::assignProcess(std::shared_ptr<Process> process) {
    std::unique_lock lock(mutex);

    if (process != nullptr) {
        process->setCPUCoreID(this->coreID);
    }
    else if (currentProcess != nullptr) {
        currentProcess->setCPUCoreID(-1);
    }

    currentProcess = process;
}

std::shared_ptr<Process> CPUWorker::getCurrentProcess() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentProcess;
}

bool CPUWorker::isFree() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentProcess == nullptr;
}

void CPUWorker::update(bool isRunning) {
    this->running = isRunning;
}

void CPUWorker::run() {
    //std::cout << "CPUWorker " << coreID << " thread started\n";
    auto g = GlobalScheduler::getInstance();
    while (running) {
        if (g->cpuCycles % (g->delaysPerExec + 1) == 0){
            std::unique_lock lock(mutex);
            if (currentProcess != nullptr && !currentProcess->isFinished()) {
                currentProcess->nextInstruction();
            }
            lock.unlock();
        }
        cpuCycles++;
        this->sleep(100);
    }
    //std::cout << "CPUWorker " << coreID << " thread stopped\n";
}

int CPUWorker::getCoreID()
{
    return coreID;
}