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
    std::shared_ptr<Process> createUniqueProcess(std::string name, size_t memoryRequired, bool generateRandomCommands = true);
    std::shared_ptr<Process> createUniqueProcess(std::string name);
    std::shared_ptr<Process> createUniqueProcess(std::string name, bool generateRandomCommands = true);
    std::vector<std::shared_ptr<CPUWorker>> getWorkers();
    void generateReport();
    void setGenerating(bool generating);
    void printConfig();

    bool isInitialized();
    bool hasStarted();
    void displayVMStat();
    void displayProcessSMI();
    friend class CPUWorker;

private:
    GlobalScheduler(Config config);
    ~GlobalScheduler() = default;
    GlobalScheduler& operator=(GlobalScheduler const&) { }
    void tick();
    void startWorkers();
    void run() override;
    uint64_t activeCpuTicks = 0;
    uint64_t idleCpuTicks = 0;
    static GlobalScheduler* sharedInstance;
    std::vector<std::shared_ptr<CPUWorker>> workers;
    std::shared_ptr<AScheduler> scheduler;
    std::shared_ptr<Process> generateProcess();
    bool generateProcesses = false;
    void runFCFS();
    void runRR();
    void updateWorkers();
    void updateSleepingProcesses();
    void generateMemLog(int cpuCycles);
};
