// config.h : Defines configuration structure and config loader interface.
//
// Description:
//   Contains the Config struct and function declarations for loading
//   and printing configuration values.
//

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