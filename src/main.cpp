#include "ConsoleManager.h"
#include "FlatMemoryAllocator.h"
#include "GlobalScheduler.h"

int main()
{
    ConsoleManager::initialize();

    bool running = true;

    while (running)
    {
        ConsoleManager::getInstance()->process();
        running = ConsoleManager::getInstance()->isRunning();
    }

    if (GlobalScheduler::getInstance() != nullptr)
        GlobalScheduler::destroy();

    if (FlatMemoryAllocator::getInstance() != nullptr)
        FlatMemoryAllocator::destroy();

    ConsoleManager::destroy();

    return 0;
}
