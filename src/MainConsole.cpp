#include <iostream>
#include <string>
#include <sstream>
#include "MainConsole.h"
#include "Config.h"
#include "ConsoleManager.h"
#include "GlobalScheduler.h"

#define CONFIG_FILE "resources/config.txt"

void printHeader();
void printCommand();

void MainConsole::display()
{
    printHeader();
}

MainConsole::MainConsole()
    : AConsole("MAIN_CONSOLE")
{
}

void MainConsole::process()
{
    std::string input;
    std::cout << "\nEnter a command: ";
    getline(std::cin, input);

    handleCommand(input);
}

void MainConsole::onEnabled()
{
    display();
}
#include "OSThread.h"
void MainConsole::handleCommand(const std::string& input) {
    std::string command;
    std::string args[2];
    std::stringstream ss(input);

    ss >> command >> args[0] >> args[1];

    //printCommand(command, args);
    if (command == "initialize") {
        if (GlobalScheduler::getInstance() != nullptr) {
            std::cout << "Already initialized.\n";
        }
        else {
            Config temp;
            if (loadConfig(CONFIG_FILE, temp) && validateConfig(temp)) {
                GlobalScheduler::init(temp);
                std::cout << "Config initialized successfully.\n";
            }
        }
    }
    else if (command == "exit") {
        GlobalScheduler::getInstance()->stop();
        ConsoleManager::getInstance()->exitApplication();
    }
    else if (command == "clear") {
        system("cls");
        display();
    }
    else if (command == "screen") {
        if (args[0] == "-ls") 
        {
            if (GlobalScheduler::getInstance() != nullptr) 
            {
                displayScreenLS();
            } 
            else 
            {
                std::cout << "Scheduler is not initialized. Please run 'initialize' first.\n";
            }
        }
        else if (args[0] == "-s")
        {
            if (args[1] == "") {
                std::cout << "Please enter the process name.\n";
            }
            else {
                std::cerr << "Here111";
                auto process = GlobalScheduler::getInstance()->findProcess(args[1]);
                if (process != nullptr) {
                    OSThread::sleep(100);
                    auto screen = std::make_shared<BaseScreen>(process, args[1]);
                    ConsoleManager::getInstance()->registerScreen(screen);
                    ConsoleManager::getInstance()->switchToScreen(screen->getName());
                }
                else {
                    std::cout << "Screen attach failed. Process " << args[1] << " not found.";
                }
             } 
        }
        else {
            std::cout << "Invalid arguments for " << command << " command.\n";
        }
    }
    else if (command == "scheduler-start") {
        if (GlobalScheduler::getInstance() == nullptr) {
            std::cout << "Config not initialized yet.\n";
        }
        else {
            GlobalScheduler::getInstance()->setGenerating(true);
            std::cout << "Scheduler started.\n";
        }
    }
    else if (command == "scheduler-stop") {
        if (GlobalScheduler::getInstance() == nullptr) {
            std::cout << "Config not initialized yet.\n";
        }
        else
        {
            GlobalScheduler::getInstance()->setGenerating(false);
            std::cout << "Scheduler stopped. \n";
        }
    }
    else if (command == "report-util") {
        if (GlobalScheduler::getInstance() != nullptr) 
        {
            GlobalScheduler::getInstance()->generateReport();
            
            std::cout << "Report generated at C:/csopesy-log.txt!\n";
        } 
        else 
        {
            std::cout << "Scheduler is not initialized. Please run 'initialize' first.\n";
        }
    }
    else std::cout << "Unknown command: " << input << std::endl;
}

//for debugging purposes only
void printCommand(std::string command, std::string args[2]) {
    std::cout << "++++++++++++++++++++++++++++++++\n";
    std::cout << "Command: " << command << std::endl;
    if (args[0] != "") {
        std::cout << "Args: " << args[0];
        if (args[1] != "")
            std::cout << ", " << args[1] << std::endl;
        else
            std::cout << "\n";
    }
    std::cout << "++++++++++++++++++++++++++++++++\n";
}


void printHeader()
{
    std::cout << "*==================================================*";
    std::cout << R"(
  _____  _____  ____  _____  ______  _______     __
 / ____|/ ____|/ __ \|  __ \|  ____|/ ____\ \   / /
| |    | (___ | |  | | |__) | |__  | (___  \ \_/ / 
| |     \___ \| |  | |  ___/|  __|  \___ \  \   /  
| |____ ____) | |__| | |    | |____ ____) |  | |   
 \_____|_____/ \____/|_|    |______|_____/   |_|   
                                                   
   )" << "\n\n";

    std::cout << "Developed by S09 Group 7\n";
    std::cout << "Almoradie, Nicole\n";
    std::cout << "Amon, Mikaela\n";
    std::cout << "Filipino, Eunice\n";
    std::cout << "Wee, Justine\n";
    std::cout << "*==================================================*";
}

void MainConsole::displayScreenLS() const
{
    auto workers = GlobalScheduler::getInstance()->getWorkers();
    int activeCores = 0;
    int totalCores = workers.size();

    for (const auto& worker : workers) 
    {
        if (!worker->isFree()) 
        {
            activeCores++;
        }
    }
    
    int cpuUtil = (totalCores > 0) ? (activeCores * 100) / totalCores : 0;

    std::cout << "CPU Utilization: " << cpuUtil << "%\n";
    std::cout << "Cores used: " << activeCores << "\n";
    std::cout << "Cores available: " << (totalCores - activeCores) << "\n";
    std::cout << "\n--------------------------------------------------\n";
    
    std::cout << "Running processes:\n";

    typedef std::deque<std::shared_ptr<Process>> Queue;
    Queue runningProcesses = GlobalScheduler::getInstance()->getRunningProcesses();

    if (!runningProcesses.empty())
    {
        for (const auto& p : runningProcesses){
            std::cout << std::left << std::setw(15) << p->getName()
                << "(" << p->getFormattedCreationTime() << ")    "
                << "Core: " << std::setw(5) << p->getCPUCoreID()
                << p->getCommandCounter() << " / " << p->getLinesOfCode() << "\n";
        }
    }
    else std::cout << "No running processes\n";

    Queue finishedProcesses = GlobalScheduler::getInstance()->getFinishedProcesses();
    std::cout << "\nFinished processes:\n";
    
    if (!finishedProcesses.empty()) {
        for (const auto& p : finishedProcesses)
        {
            std::cout << std::left << std::setw(15) << p->getName()
                << "(" << p->getFormattedCreationTime() << ")    "
                << std::setw(12) << "Finished"
                << p->getCommandCounter() << " / " << p->getLinesOfCode() << "\n";
        }
    }
    else std::cout << "No finished processes\n";
    
    std::cout << "--------------------------------------------------\n";
}
