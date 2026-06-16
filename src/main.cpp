#include "ConsoleManager.h"
#include "GlobalScheduler.h"

int main()
{
    ConsoleManager::initialize();

    bool running = true;

    while (running)
    {
        ConsoleManager::getInstance()->process();
        //ConsoleManager::getInstance()->drawConsole();
		
        running = ConsoleManager::getInstance()->isRunning();
    }

    if (GlobalScheduler::getInstance() != nullptr)
        GlobalScheduler::getInstance()->destroy();
    ConsoleManager::destroy();
    return 0;
}
