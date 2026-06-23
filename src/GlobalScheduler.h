#pragma once
#include <random>
#include <string>
#include <memory>
#include "CPUWorker.h"
#include "AScheduler.h"

class GlobalScheduler : public AScheduler {
public:
    static GlobalScheduler* getInstance();
    static void initialize(Config config);
    static void destroy();

    void init() override;
    std::shared_ptr<Process> createUniqueProcess(std::string name);
    std::shared_ptr<Process> findProcess(std::string name) const;
    std::vector<std::shared_ptr<CPUWorker>> getWorkers();
    void tick();

    void run() override;
    void stop();
    //bool generateReport();
    void printConfig();
    bool isInitialized();

private:
    GlobalScheduler(Config config);
    ~GlobalScheduler() = default;
    GlobalScheduler& operator=(GlobalScheduler const&) { }
    static GlobalScheduler* sharedInstance;
    std::vector<std::shared_ptr<CPUWorker>> workers;
};
