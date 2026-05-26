#define CONFIG_FILE "resources/config.txt"

#include "System.h"
#include "Config.h"

#include <iostream>

System* System::sharedInstance = nullptr;
System* System::getInstance()
{
    return sharedInstance;
}

void System::initialize()
{
    sharedInstance = new System();
}

void System::destroy()
{
    delete sharedInstance;
}

void System::initializeConfig() {
    if (!configInitialized) {
        Config temp;
        if (loadConfig(CONFIG_FILE, temp)) {

            if (validateConfig(temp)) {
                config = temp;
                configInitialized = true;
                std::cout << "Config initialized\n";

                //printConfig(config);
            }
        }
    }
    else std::cout << "Config already initialized\n";
};

bool System::isConfigInitialized() {
    return configInitialized;
}


Config& System::getConfig() {
    return config;
};

void System::startScheduler() {
    scheduler.start(configInitialized);
}

void System::stopScheduler() {
    scheduler.stop(configInitialized);
}

bool System::generateReport() {
    //put logic here

    //plalceholder
    return true;
}

System::System()
{
	
}
