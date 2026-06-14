#include "GlobalScheduler.h"
#include <iostream>

GlobalScheduler::GlobalScheduler(Config config)
	: AScheduler(config)
{ }

void GlobalScheduler::run()
{
	std::cout << "thread running\n";
}

// static instance
GlobalScheduler* GlobalScheduler::sharedInstance = nullptr;

GlobalScheduler* GlobalScheduler::getInstance()
{
	return sharedInstance;
}

void GlobalScheduler::initialize(Config config)
{
	if (!sharedInstance)
		sharedInstance = new GlobalScheduler(config);
}

void GlobalScheduler::destroy()
{
	delete sharedInstance;
	sharedInstance = nullptr;
}

void GlobalScheduler::init()
{
	// placeholder initialization logic
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
