#include "GlobalScheduler.h"
#include <iostream>

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
	std::cout << "GlobalScheduler thread started\n";
	while (running)
	{
		for (auto& worker : workers) {
			if (!worker->isFree() && worker->getCurrentProcess()->isFinished()) {
				finishedProcesses.push_back(worker->getCurrentProcess());
				worker->assignProcess(nullptr); // free the worker
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
