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
		if (generateProcesses) tick();

		//check workers if current process is finished executing & update accordingly
		//also updates the queue
		updateWorkers();
		updateWaitingProcesses();

		if (algo == SchedulingAlgorithm::FCFS)
		{
			runFCFS();
		}
		else if (algo == SchedulingAlgorithm::RR)
		{
			runRR();
		}
		
		this->sleep(100);
		cpuCycles++;
	}
}

void GlobalScheduler::runFCFS()
{
	//First come first serve: simply iterate through CPU workers and add processes in the queue
	for (auto& worker : workers) {
		if (worker->isFree() && !readyQueue.empty()) {
			std::unique_lock lock(mutex);
			auto process = readyQueue.front();
			readyQueue.pop_front();
			runningProcesses.push_back(process);
			lock.unlock();
			worker->assignProcess(process);
		}
	}
}

void GlobalScheduler::runRR()
{
	for (auto& worker : workers) {
		
		auto process = worker->getCurrentProcess();

		if (!worker->isFree()) {
			auto process = worker->getCurrentProcess();
			if (process->getCyclesInCPU() >= quantumCycles)
			{
				//only preempt if there is a process to switch to
				if (!readyQueue.empty())
				{
					//pause process (change state from RUNNING to READY)
					process->pauseProcess();
					process->setCPUCoreID(-1);

					std::unique_lock lock(mutex);

					//place in process in ready queue
					readyQueue.push_back(process);
					// Remove from running list
					auto it = std::find(runningProcesses.begin(), runningProcesses.end(), process);
					if (it != runningProcesses.end()) {
						runningProcesses.erase(it);
					}

					lock.unlock();
					worker->assignProcess(nullptr); // Free the worker
				}
				//reset this whether or not RQ is empty.
				process->resetCyclesInCPU();
			}
		}

		//if the current worker (could be the currently preempted one) is free, add a new process
		if (worker->isFree() && !readyQueue.empty())
		{
			std::unique_lock lock(mutex);

			auto process = readyQueue.front();
			readyQueue.pop_front();

			runningProcesses.push_back(process);

			lock.unlock();

			worker->assignProcess(process);
		}
	}
}


void GlobalScheduler::updateWorkers()
{
	for (auto& worker : workers) {
		if (worker->isFree()) {
			continue; // Safely skip free workers
		}

		auto currentProc = worker->getCurrentProcess();

		if (currentProc->isFinished()) {

			std::unique_lock lock(mutex);
			// Add to finished list
			finishedProcesses.push_back(currentProc);

			// Remove from running list
			auto it = std::find(runningProcesses.begin(), runningProcesses.end(), currentProc);
			if (it != runningProcesses.end()) {
				runningProcesses.erase(it);
			}
			lock.unlock();
			worker->assignProcess(nullptr); // Free the worker
		}
		//handle WAITING state (triggered by SLEEP(X))
		else if (currentProc->getState() == ProcessState::WAITING) {
			currentProc->setCPUCoreID(-1);
			currentProc->resetCyclesInCPU(); // Reset counter for clean tracking

			std::unique_lock lock(mutex);
			waitingProcesses.push_back(currentProc); // Move to sleeping track

			auto it = std::find(runningProcesses.begin(), runningProcesses.end(), currentProc);
			if (it != runningProcesses.end()) {
				runningProcesses.erase(it);
			}
			lock.unlock();
			worker->assignProcess(nullptr);
		}
		else {
			worker->getCurrentProcess()->incrementCyclesInCPU();
		}
	}
}

void GlobalScheduler::updateWaitingProcesses()
{
	std::unique_lock lock(mutex);
	for (auto it = waitingProcesses.begin(); it != waitingProcesses.end(); ) {
		auto process = *it;

		// 1. Decrement the sleep timer by 1 tick
		process->decrementSleepTicks();

		// 2. Check if it's time to wake up
		if (process->getRemainingSleepTicks() <= 0) {
			readyQueue.push_back(process);

			// Remove it from the sleeping list safely mid-iteration
			it = waitingProcesses.erase(it);
		}
		else {
			++it; // Move to the next sleeping process
		}
	}
	lock.unlock();
}

GlobalScheduler* GlobalScheduler::getInstance()
{
	return sharedInstance;
}

void GlobalScheduler::init(Config config) {
	if (!sharedInstance) {
		sharedInstance = new GlobalScheduler(config);
	}
	sharedInstance->startWorkers();
	sharedInstance->start();
}

void GlobalScheduler::startWorkers()
{
	// start workers
	for (auto& worker : workers) {
		worker->update(true);
		worker->start();
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
}

void GlobalScheduler::setGenerating(bool generating)
{
	generateProcesses = generating;
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
