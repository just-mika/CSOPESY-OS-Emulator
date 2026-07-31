#include "GlobalScheduler.h"

#include <filesystem>

#include "PagedMemoryAllocator.h"
#include <iostream>
#include <iomanip> // for std::setw and std::left
#include <fstream>

#include "CPUWorker.h"
#include "BackingStore.h"

class ForCommand;
GlobalScheduler* GlobalScheduler::sharedInstance = nullptr;
static int pidCounter = 0;

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
		if (generateProcesses) {
			tick();

			updateWorkers();
			updateSleepingProcesses();
			checkMemoryBlockedQueue();

			if (algo == SchedulingAlgorithm::FCFS) {
				runFCFS();
			}
			else if (algo == SchedulingAlgorithm::RR) {
				runRR();
			}

			cpuCycles++;

			if (cpuCycles > 0 && cpuCycles % quantumCycles == 0) {
				generateMemLog(cpuCycles);
			}
		}

		this->sleep(100);
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
					
					std::unique_lock lock(mutex);

					// Add back to readyQueue
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
			PagedMemoryAllocator::getInstance()->deallocate(currentProc->getMemoryAddress());
			currentProc->setMemoryAddress(nullptr);

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

			checkMemoryBlockedQueue();
		}
		// handle WAITING state (triggered by SLEEP(X)); do not include unallocated processes
		else if (currentProc->getState() == ProcessState::WAITING && currentProc->getMemoryAddress() != nullptr) {
			currentProc->resetCyclesInCPU(); // Reset counter for clean tracking

			std::unique_lock lock(mutex);
			sleepingProcesses.push_back(currentProc); // Move to sleeping track

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

// Sleep Command
void GlobalScheduler::updateSleepingProcesses()
{
	std::unique_lock lock(mutex);
	for (auto it = sleepingProcesses.begin(); it != sleepingProcesses.end(); ) {
		auto process = *it;

		// 1. Decrement the sleep timer by 1 tick
		process->decrementSleepTicks();

		// 2. Check if it's time to wake up
		if (process->getRemainingSleepTicks() <= 0) {
			process->setState(ProcessState::READY);
			readyQueue.push_back(process);
			// Remove it from the sleeping list safely mid-iteration
			it = sleepingProcesses.erase(it);
		}
		else {
			++it; // Move to the next sleeping process
		}
	}
	lock.unlock();
	checkMemoryBlockedQueue();
}

GlobalScheduler* GlobalScheduler::getInstance()
{
	return sharedInstance;
}

void GlobalScheduler::init(Config config) {
	std::string fileName = "csopesy-backing-store.txt";
	if (!sharedInstance) {
		sharedInstance = new GlobalScheduler(config);
	}
	// Initialize BackingStore
	BackingStore disk(fileName, sharedInstance->AScheduler::memPerFrame);

	// Initialize Memory Allocator First
	PagedMemoryAllocator::init(config.maxOverallMem, config.memPerFrame);
	sharedInstance->memoryAllocator = std::shared_ptr<IMemoryAllocator>(PagedMemoryAllocator::getInstance(), [](IMemoryAllocator*) {});


	// Start the Workers
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

//std::shared_ptr<Process> GlobalScheduler::createUniqueProcess(std::string name)
//{
//	auto process = std::make_shared<Process>(++nextPID, name, AScheduler::memPerProc);
//	int totalCommands = (rand() % (maxIns - minIns + 1)) + minIns;
//	process->initializeCommands(totalCommands);
//	addProcess(process);
//	return process;
//}

std::shared_ptr<Process> GlobalScheduler::createUniqueProcess(std::string name, size_t memoryRequired, bool generateRandomCommands)
{
	// Uses your exact PID counter ++nextPID and memory allocation
	auto process = std::make_shared<Process>(++nextPID, name, memoryRequired);

	// Generate random commands for 'screen -s'
	if (generateRandomCommands) {
		int totalCommands = (rand() % (maxIns - minIns + 1)) + minIns;
		process->initializeCommands(totalCommands);
	}

	// Properly enqueue into scheduler ready queue
	addProcess(process);
	return process;
}

std::shared_ptr<Process> GlobalScheduler::createUniqueProcess(std::string name)
{
	// Default 1-argument overload using min memory requirement for process creation
	return createUniqueProcess(name, AScheduler::minMemPerProc, true);
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
	std::string processName = "process_";
	if (++nextPID < 10) processName += "0";
	processName += std::to_string(nextPID);

	size_t rolledMem = AScheduler::minMemPerProc;
	if (AScheduler::maxMemPerProc > AScheduler::minMemPerProc) {
		rolledMem = AScheduler::minMemPerProc + (rand() % (AScheduler::maxMemPerProc - AScheduler::minMemPerProc + 1));
	}

	std::shared_ptr<Process> newProcess = std::make_shared<Process>(nextPID, processName, rolledMem);
	//std::shared_ptr<Process> newProcess = std::make_shared<Process>(nextPID, processName, AScheduler::memPerProc);

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
void GlobalScheduler::generateReport() 
{
    std::ofstream outFile("csopesy-log.txt");
    if (!outFile.is_open()) 
    {
        std::cout << "Error: Unable to generate csopesy-log.txt\n";
        return;
    }

    int activeCores = 0;
    for (const auto& worker : workers) 
    {
        if (!worker->isFree()) 
        {
            activeCores++;
        }
    }
    
    int totalCores = workers.size();
    int cpuUtil = (totalCores > 0) ? (activeCores * 100) / totalCores : 0;

    outFile << "CPU Utilization: " << cpuUtil << "%\n";
    outFile << "Cores used: " << activeCores << "\n";
    outFile << "Cores available: " << totalCores - activeCores << "\n";
    outFile << "\n--------------------------------------------------\n";
    
    outFile << "Running processes:\n";
    auto running = this->getRunningProcesses();
    if (!running.empty()) 
    {
        for (const auto& p : running) 
        {
            outFile << std::left << std::setw(15) << p->getName()
                    << "(" << p->getCreatedTime() << ")    "
                    << "Core: " << std::setw(5) << p->getCPUCoreID()
                    << p->getCommandCounter() << " / " << p->getLinesOfCode() << "\n";
        }
    } 
    else 
    {
        outFile << "No running processes\n";
    }

    outFile << "\nFinished processes:\n";
    auto finished = this->getFinishedProcesses();
    if (!finished.empty()) 
    {
        for (const auto& p : finished) 
        {
            outFile << std::left << std::setw(15) << p->getName()
                    << "(" << p->getCreatedTime() << ")    "
                    << std::setw(12) << "Finished"
                    << p->getCommandCounter() << " / " << p->getLinesOfCode() << "\n";
        }
    } 
    else 
    {
        outFile << "No finished processes\n";
    }

    outFile << "--------------------------------------------------\n";
    outFile.close();
}

bool GlobalScheduler::hasStarted()
{
	return generateProcesses;
}

static std::string formatSnapshotTime(std::time_t t) {
	std::tm tm_struct;
#ifdef _WIN32
	localtime_s(&tm_struct, &t);
#else
	localtime_r(&t, &tm_struct);
#endif
	std::stringstream ss;
	ss << std::put_time(&tm_struct, "%m/%d/%Y %I:%M:%S%p");
	return ss.str();
}

void GlobalScheduler::generateMemLog(int cpuCycles) {
	//inititalize file
	std::string DIRECTORY_PATH = "output/mem_snapshots/";

	try {
		if (!std::filesystem::exists(DIRECTORY_PATH)) {
			std::filesystem::create_directories(DIRECTORY_PATH);
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "[Logger Error] Directory creation failed: " << e.what() << std::endl;
	}

	std::string fullFilePath = DIRECTORY_PATH + "memory_stamp_" + std::to_string(cpuCycles) + ".txt";
	std::ofstream outFile(fullFilePath, std::ios::out);
	/*
	if (outFile.is_open()) {
		auto blocks = FlatMemoryAllocator::getInstance()->getAllocatedBlocks();

		//sort blocks by index in descending order
		std::sort(blocks.begin(), blocks.end(),
			[](const AllocatedBlock& a, const AllocatedBlock& b) { return a.index > b.index; });

		//build the block listing first, tallying fragmentation as we go
		std::ostringstream blockListing;
		size_t fragmentation = 0;
		size_t topOfGap = maxOverallMem; // starts at the very top of memory

		for (auto& b : blocks) {
			size_t blockTop = b.index + b.size;
			if (topOfGap > blockTop) {
				fragmentation += (topOfGap - blockTop); // gap ABOVE this block
			}
			blockListing << blockTop << "\n" << b.name << "\n" << b.index << "\n\n";
			topOfGap = b.index; // next gap ends where this block starts
		}
		

		//get timestamp
		std::time_t now = std::time(nullptr);
		std::tm timeInfo{};
		localtime_s(&timeInfo, &now);

		outFile << "Timestamp: (" << std::put_time(&timeInfo, "%m/%d/%Y %I:%M:%S %p") << ")\n";
		outFile << "Number of processes in memory: " << blocks.size() << "\n";
		outFile << "Total external fragmentation in KB: " << fragmentation << "\n\n";
		outFile << "----end---- = " << maxOverallMem << "\n\n";

		//iterate through blocks and print details
		for (auto& b : blocks) {
			outFile << (b.index + b.size) << "\n" << b.name << "\n" << b.index << "\n\n";
		}

		outFile << "----start----- = 0\n";
		outFile.close();
	}
	else {
		std::cerr << "[Memory Logger] Unable to initialize file at: " << fullFilePath << std::endl;
	}
	*/
}