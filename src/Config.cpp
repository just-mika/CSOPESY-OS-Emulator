#include "Config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>

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
	}

	file.close();
	return true;
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

	if (config.quantumCycles < 1 || config.quantumCycles > 4294967296) {
		std::cout << "Invalid quantum-cycles value\n";
		return false;
	}

	if (config.batchProcessFreq < 1 || config.batchProcessFreq > 4294967296) {
		std::cout << "Invalid batch-process-freq value\n";
		return false;
	}

	if (config.minIns < 1 || config.minIns > 4294967296) {
		std::cout << "Invalid min-ins value\n";
		return false;
	}

	if (config.maxIns < 1 || config.maxIns > 4294967296) {
		std::cout << "Invalid max-ins value\n";
		return false;
	}

	if (config.delaysPerExec < 0 || config.delaysPerExec > 4294967296) {
		std::cout << "Invalid delay-per-exec value\n";
		return false;
	}

	if ((config.maxIns < config.minIns)) {
		std::cout << "min-in value is greater than max-in value\n";
		return false;
	}

	return true;
}

//for debugging purposes only
void printConfig(Config config) {
	std::cout << "++++++++++++++++++++++++++++++++\n";
	std::cout << "num-cpu: " << config.numCPU << std::endl;
	std::cout << "scheduler: " << config.scheduler << std::endl;
	std::cout << "quantum-cycles: " << config.quantumCycles << std::endl;
	std::cout << "batch-process-freq: " << config.batchProcessFreq << std::endl;
	std::cout << "min-ins: " << config.minIns << std::endl;
	std::cout << "max-ins: " << config.maxIns << std::endl;
	std::cout << "delay-per-exec: " << config.delaysPerExec << std::endl;
	std::cout << "++++++++++++++++++++++++++++++++\n";
}
