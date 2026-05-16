#pragma once
#include <string>
#include <cstdint>

struct Config {
	int numCPU;
	std::string scheduler;
	unsigned long long quantumCycles;
	unsigned long long batchProcessFreq;
	unsigned long long minIns;
	unsigned long long maxIns;
	unsigned long long delaysPerExec;
};

bool loadConfig(std::string filename, Config& config);

void printConfig(Config config);