#include "GlobalScheduler.h"
#include <iostream>
#include <iomanip> // for std::setw and std::left

#include "CPUWorker.h"

GlobalScheduler* GlobalScheduler::sharedInstance = nullptr;

GlobalScheduler::GlobalScheduler(Config config)
	: AScheduler(config)
{
	// initialize CPUWorkers here
	for (int i = 0; i < numCPU; i++) {
		workers.push_back(std::make_shared<CPUWorker>(i));
	}
}

void GlobalScheduler::run()
{
	running = true;
	//std::cout << "GlobalScheduler thread started\n";
	while (running)
	{
		for (auto& worker : workers) {
			if (!worker->isFree() && worker->getCurrentProcess()->isFinished()) {

				auto finishedProc = worker->getCurrentProcess();

				// Add to finished list
				finishedProcesses.push_back(finishedProc);

				// Remove from running list
				auto it = std::find(runningProcesses.begin(), runningProcesses.end(), finishedProc);
				if (it != runningProcesses.end()) {
					runningProcesses.erase(it);
				}

				worker->assignProcess(nullptr); // Free the worker
			}
		}

		for (auto& worker : workers) {
			if (worker->isFree() && !readyQueue.empty()) {
				auto process = readyQueue.front();
				readyQueue.pop_front();
				runningProcesses.push_back(process);
				worker->assignProcess(process);
			}
		}
		OSThread::sleep(100);
	}
}

GlobalScheduler* GlobalScheduler::getInstance()
{
	return sharedInstance;
}

void GlobalScheduler::initialize(Config config) {
	if (!sharedInstance) {
		sharedInstance = new GlobalScheduler(config);
	}
}

void GlobalScheduler::init()
{
	// start workers
	for (auto& worker : workers) {
		worker->update(true);
		worker->start();
	}
	this->start(); // start scheduler thread

	// create 10 dummy processes
	for (int i = 1; i <= 10; ++i) {
		std::string processName = "screen_";
		if (i < 10) processName += "0";
		processName += std::to_string(i);

		std::shared_ptr<Process> newProcess = std::make_shared<Process>(i, processName);

		// Load the 100 commands into this process
		newProcess->initializeCommands(100);

		// Add to the Ready Queue
		this->addProcess(newProcess);
	}
}

void GlobalScheduler::destroy()
{
	delete sharedInstance;
	sharedInstance = nullptr;
}

std::shared_ptr<Process> GlobalScheduler::createUniqueProcess(std::string name)
{
	static int pidCounter = 0;
	auto process = std::make_shared<Process>(pidCounter++, name);
	addProcess(process);
	return process;
}

//for debugging purposes only
void GlobalScheduler::printConfig() {
	std::cout << "++++++++++++++++++++++++++++++++\n";
	std::cout << "num-cpu: " << numCPU << std::endl;
	std::cout << "scheduler: " << algo << std::endl;
	std::cout << "quantum-cycles: " << quantumCycles << std::endl;
	std::cout << "batch-process-freq: " << batchProcessFreq << std::endl;
	std::cout << "min-ins: " << minIns << std::endl;
	std::cout << "max-ins: " << maxIns << std::endl;
	std::cout << "delay-per-exec: " << delaysPerExec << std::endl;
	std::cout << "++++++++++++++++++++++++++++++++\n";
}


void GlobalScheduler::displayScreenLS() const 
{
    std::cout << "--------------------------------------------------\n";
    std::cout << "Running processes:\n";
    
    // Loop through CPU workers to find actively executing processes
    for (const auto& worker : workers) 
    {
        if (!worker->isFree()) 
        {
            auto p = worker->getCurrentProcess();
            if (p != nullptr) 
            {
				//Reference: https://en.cppreference.com/cpp/io/manip/left
                std::cout << std::left << std::setw(15) << p->getName()
                          << "(" << p->getFormattedCreationTime() << ")    "
                          << "Core: " << std::setw(5) << p->getCPUCoreID() 
                          << p->getCommandCounter() << " / " << p->getLinesOfCode() << "\n";
            }
        }
    }

    std::cout << "\nFinished processes:\n";
    for (const auto& p : finishedProcesses) 
    {
        std::cout << std::left << std::setw(15) << p->getName()
                  << "(" << p->getFormattedCreationTime() << ")    "
                  << std::setw(12) << "Finished"
                  << p->getCommandCounter() << " / " << p->getLinesOfCode() << "\n";
    }
    std::cout << "--------------------------------------------------\n";
}
