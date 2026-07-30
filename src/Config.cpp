#include "Config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#define CONFIG_FILE "resources/config.txt"
#define MAX_RANGE 4294967296

// All memory ranges are [2^6, 2^16] and the power of 2 format
#define MIN_MEM_RANGE 64ULL
#define MAX_MEM_RANGE 65536ULL

static Config globalConfig;
static bool configInitialized = false;

bool loadConfig(std::string filename, Config& config) {
	std::ifstream file(filename);

	if (!file.is_open()) {
		std::cerr << "Unable to open file" << std::endl;
		return false;
	}

	std::string key, value;

	for (std::string line; std::getline(file, line);) {
		std::stringstream ss(line);

		ss >> key >> value;

		if (key == "num-cpu")
			config.numCPU = std::stoi(value);

		else if (key == "scheduler")
			config.scheduler = value;

		else if (key == "quantum-cycles")
			config.quantumCycles = std::stoull(value);

		else if (key == "batch-process-freq")
			config.batchProcessFreq = std::stoull(value);

		else if (key == "min-ins")
			config.minIns = std::stoull(value);

		else if (key == "max-ins")
			config.maxIns = std::stoull(value);

		else if (key == "delay-per-exec")
			config.delaysPerExec = std::stoull(value);
			
		else if (key == "max-overall-mem")
			config.maxOverallMem = std::stoull(value);

		else if (key == "mem-per-frame")
			config.memPerFrame = std::stoull(value);

		else if (key == "min-mem-per-proc")
			config.minMemPerProc = std::stoull(value);

		else if (key == "max-mem-per-proc")
			config.maxMemPerProc = std::stoull(value);
	}

	file.close();
	return true;
}


bool isValidMemorySize(unsigned long long size) {
	if (size < MIN_MEM_RANGE || size > MAX_MEM_RANGE) {
		return false;
	}
	// power of two check
	return (size & (size - 1)) == 0;
}

bool validateConfig(Config config) {
	if (config.numCPU < 1 || config.numCPU > 128) {
		std::cout << "Invalid num-cpu value\n";
		return false;
	}

	if (config.scheduler != "\"rr\"" && config.scheduler != "\"fcfs\"") {
		std::cout << "Invalid scheduler algorithm\n";
		return false;
	}

	if (config.quantumCycles < 1 || config.quantumCycles > MAX_RANGE) {
		std::cout << "Invalid quantum-cycles value\n";
		return false;
	}

	if (config.batchProcessFreq < 1 || config.batchProcessFreq > MAX_RANGE) {
		std::cout << "Invalid batch-process-freq value\n";
		return false;
	}

	if (config.minIns < 1 || config.minIns > MAX_RANGE) {
		std::cout << "Invalid min-ins value\n";
		return false;
	}

	if (config.maxIns < 1 || config.maxIns > MAX_RANGE) {
		std::cout << "Invalid max-ins value\n";
		return false;
	}

	if (config.delaysPerExec < 0 || config.delaysPerExec > MAX_RANGE) {
		std::cout << "Invalid delay-per-exec value\n";
		return false;
	}

	if ((config.maxIns < config.minIns)) {
		std::cout << "min-in value is greater than max-in value\n";
		return false;
	}

	if (!isValidMemorySize(config.maxOverallMem)) {
		std::cout << "Invalid max-overall-mem value\n";
		return false;
	}

	if (!isValidMemorySize(config.memPerFrame)) {
		std::cout << "Invalid mem-per-frame value\n";
		return false;
	}

	if (!isValidMemorySize(config.minMemPerProc)) {
		std::cout << "Invalid min-mem-per-proc value\n";
		return false;
	}

	if (!isValidMemorySize(config.maxMemPerProc)) {
		std::cout << "Invalid max-mem-per-proc value\n";
		return false;
	}

	if (config.maxMemPerProc < config.minMemPerProc) {
		std::cout << "min-mem-per-proc value is greater than max-mem-per-proc value\n";
		return false;
	}

	return true;
}

bool isConfigInitialized() { return configInitialized; }

void initializeConfig() { configInitialized = true; }

Config& getConfig() { return globalConfig; }
