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
	unsigned long long memPerProc;
};

bool loadConfig(std::string filename, Config& config);

bool validateConfig(Config config);

void printConfig(Config config);

void initializeConfig();

bool isConfigInitialized();

Config& getConfig();
