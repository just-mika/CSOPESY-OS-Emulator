#pragma once
#include <random>
#include <string>
#include <memory>
#include "CPUWorker.h"
#include "AScheduler.h"

class GlobalScheduler : public AScheduler {
public:
    static GlobalScheduler* getInstance();
    static void destroy();

    static void init(Config config);
    std::shared_ptr<Process> createUniqueProcess(std::string name);
    std::vector<std::shared_ptr<CPUWorker>> getWorkers();
    void generateReport();
    void setGenerating(bool generating);
    void printConfig();
    bool isInitialized();
    bool hasStarted();

    friend class CPUWorker;

private:
    GlobalScheduler(Config config);
    ~GlobalScheduler() = default;
    GlobalScheduler& operator=(GlobalScheduler const&) { }
    void tick();
    void startWorkers();
    void run() override;
    static GlobalScheduler* sharedInstance;
    std::vector<std::shared_ptr<CPUWorker>> workers;
    std::shared_ptr<AScheduler> scheduler;
    std::shared_ptr<Process> generateProcess();
    bool generateProcesses = false;
    void runFCFS();
    void runRR();
    void updateWorkers();
    void updateWaitingProcesses();
};
