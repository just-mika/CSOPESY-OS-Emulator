#pragma once
#include "Config.h"
#include "Scheduler.h"

class System {
private:
    bool initialized = false;
    bool running = true;

    Config config;
    Scheduler scheduler;

    //add more variables

public:
    void initialize();
    void shutdown();

    bool isInitialized();
    bool isRunning();

    Config& getConfig();

    void startScheduler();
    void stopScheduler();

    bool generateReport();

    //add more methods
};