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
    currentProcess = process;
}

std::shared_ptr<Process> CPUWorker::getCurrentProcess() const {
    return currentProcess;
}

bool CPUWorker::isFree() const {
    return currentProcess == nullptr;
}

void CPUWorker::update(bool isRunning) {
    this->running = isRunning;
}

void CPUWorker::run() {
    std::cout << "CPUWorker " << coreID << " thread started\n";
    while (running) {
        if (currentProcess != nullptr) {
            currentProcess->nextInstruction();
            if (currentProcess->isFinished()) {
                currentProcess = nullptr;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "CPUWorker " << coreID << " thread stopped\n";
}