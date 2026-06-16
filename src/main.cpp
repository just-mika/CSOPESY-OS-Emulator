#include "ConsoleManager.h"
#include "GlobalScheduler.h"

int main()
{
    ConsoleManager::initialize();

    bool running = true;
    int cpuCycles = 0;

    while (running)
    {
        ConsoleManager::getInstance()->process();

        cpuCycles++;
        running = ConsoleManager::getInstance()->isRunning();
    }

    if (GlobalScheduler::getInstance() != nullptr)
        GlobalScheduler::destroy();
    ConsoleManager::destroy();

    return 0;
}
