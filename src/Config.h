#pragma once
#include <string>


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

bool validateConfig(Config config);

void printConfig(Config config);