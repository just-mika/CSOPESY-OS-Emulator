#include "System.h"
#include "ConsoleManager.h"

int main()
{
    System::initialize();
    ConsoleManager::initialize();

    bool running = true;

    while (running)
    {
        ConsoleManager::getInstance()->process();
        //ConsoleManager::getInstance()->drawConsole();
		
        running = ConsoleManager::getInstance()->isRunning();
    }
    
    ConsoleManager::destroy();
    return 0;
}