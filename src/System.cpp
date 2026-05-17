#define CONFIG_FILE "resources/config.txt"

#include "System.h"
#include "Config.h"

#include <iostream>

void System::initialize() {
    if (!initialized) {
        Config temp;
        if (loadConfig(CONFIG_FILE, temp)) {

            if (validateConfig(temp)) {
                config = temp;
                initialized = true;
                std::cout << "Config initialized\n";

                //printConfig(config);
            }
        }
    }
    else std::cout << "Config already initialized\n";
};
void System::shutdown() {
    running = false;
};

bool System::isInitialized() {
    return initialized;
}

bool System::isRunning() {
    return running;
};

Config& System::getConfig() {
    return config;
};

void System::startScheduler() {
    scheduler.start(initialized);
}

void System::stopScheduler() {
    scheduler.stop(initialized);
}

bool System::generateReport() {
    //put logic here

    //plalceholder
    return true;
}