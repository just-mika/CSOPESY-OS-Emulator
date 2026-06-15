#include "AScheduler.h"

SchedulingAlgorithm parseAlgorithm(std::string algo) {
	if (algo == "\"fcfs\"")
		return FCFS;
	else if (algo == "\"sjf-preemptive\"")
		return SJF_PREEMPTIVE;
	else if (algo == "\"sjf-nonpreemptive\"")
		return SJF_NONPREEMPTIVE;
	else if (algo == "\"rr\"")
		return RR;
	else
		throw std::invalid_argument("Invalid scheduling algorithm");
}

AScheduler::AScheduler(Config config)
    : OSThread(),
    algo(parseAlgorithm(config.scheduler)),
    numCPU(config.numCPU),
    quantumCycles(config.quantumCycles),
    batchProcessFreq(config.batchProcessFreq),
    minIns(config.minIns),
    maxIns(config.maxIns),
    delaysPerExec(config.delaysPerExec)
{ }

void AScheduler::run()
{
	//std::cout << "thread running\n";
}