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
	// memory
	unsigned long long maxOverallMem;
	unsigned long long memPerFrame;
	unsigned long long minMemPerProc;
	unsigned long long maxMemPerProc;

};

bool loadConfig(std::string filename, Config& config);

bool validateConfig(Config config);

void printConfig(Config config);

void initializeConfig();

bool isConfigInitialized();

Config& getConfig();

// All memory ranges are [2^6, 2^16] and the power of 2 format
bool isValidMemorySize(unsigned long long size);
