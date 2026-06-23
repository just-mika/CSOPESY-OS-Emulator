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
	while (running)
	{
		tick();
		for (auto& worker : workers) {
			if (!worker->isFree() && worker->getCurrentProcess()->isFinished()) {

				auto finishedProc = worker->getCurrentProcess();

				std::unique_lock lock(mutex);
				// Add to finished list
				finishedProcesses.push_back(finishedProc);

				// Remove from running list
				auto it = std::find(runningProcesses.begin(), runningProcesses.end(), finishedProc);
				if (it != runningProcesses.end()) {
					runningProcesses.erase(it);
				}
				lock.unlock();
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
		this->sleep(100);
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

std::vector<std::shared_ptr<CPUWorker>> GlobalScheduler::getWorkers()
{
	return workers;
}

void GlobalScheduler::tick()
{
	if (cpuCycles % batchProcessFreq == 0) {
		std::shared_ptr<Process> newProcess = generateProcess();

		// Add to the Ready Queue
		this->addProcess(newProcess);
		//std::cout << "Process " << newProcess->getName() << " created with " << newProcess->getLinesOfCode() << " instructions \n";
	}
	cpuCycles++;
}


std::shared_ptr<Process> GlobalScheduler::generateProcess()
{
	std::string processName = "screen_";
	if (++nextPID < 10) processName += "0";
	processName += std::to_string(nextPID);

	std::shared_ptr<Process> newProcess = std::make_shared<Process>(nextPID, processName);

	int totalCommands = (rand() % (maxIns-minIns + 1)) + minIns;

	newProcess->initializeCommands(totalCommands);

	return newProcess;
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
