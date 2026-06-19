#include "CPUWorker.h"
#include <iostream>
#include <thread>

CPUWorker::CPUWorker(int coreID)
    : coreID(coreID)
{
    this->running = false;
    this->currentProcess = nullptr;
}

void CPUWorker::assignProcess(std::shared_ptr<Process> process) {
    std::unique_lock lock(mutex);
    currentProcess = process;
    if (currentProcess != nullptr) {
		currentProcess->setCPUCoreID(this->coreID); // Assign the core ID to the process
    }
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
    while (running) {
        std::unique_lock lock(mutex);
        if (currentProcess != nullptr && !currentProcess->isFinished()) {
            currentProcess->nextInstruction();
        }
        lock.unlock();

        this->sleep(100);
    }
    std::cout << "CPUWorker " << coreID << " thread stopped\n";
}