#pragma once
#include "Config.h"
#include "Scheduler.h"

class System {
private:
    System();
    ~System() = default;
    System(System const&) {} //copy constructor is private
    System& operator = (System const&) {} //assignment operator is private
    static System* sharedInstance;

    bool configInitialized = false;
    bool running = true;

    Config config;
    Scheduler scheduler;

    //add more variables

public:
	static System* getInstance();
	static void initialize();
	static void destroy();

    void initializeConfig();

    bool isConfigInitialized();

    Config& getConfig();

    void startScheduler();
    void stopScheduler();

    bool generateReport();

    //add more methods
};