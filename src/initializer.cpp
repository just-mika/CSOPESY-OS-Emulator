#define CONFIG_FILE "resources/config.txt"

#include "initializer.h"
#include "system_state.h"
#include "config.h"
#include <iostream>
#include <string>

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

void initialize(SystemState &state) {
    if (!state.initialized) {
        Config tempConfig;
        if(loadConfig(CONFIG_FILE, tempConfig)){

            if (validateConfig(tempConfig)) {
                state.config = tempConfig;
                state.initialized = true;
                std::cout << "Config initialized\n";

                //printConfig(state.config);
            }
        }
    }
    else std::cout << "Config already initialized\n";
}