#include "CPUWorker.h"
#include <iostream>
#include <thread>

CPUWorker::CPUWorker(int coreID)
    : coreID(coreID)
{
}

void CPUWorker::assignProcess(std::shared_ptr<Process> process) {

}

std::shared_ptr<Process> CPUWorker::getCurrentProcess() const {
}

bool CPUWorker::isFree() const {
}

void CPUWorker::update(bool isRunning) {
}

void CPUWorker::run() {
    std::cout << "CPUWorker " << coreID << " thread started\n";
}