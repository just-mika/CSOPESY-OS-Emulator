#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <algorithm>
#include "MainConsole.h"
#include "Config.h"
#include "ConsoleManager.h"
#include "FlatMemoryAllocator.h"
#include "GlobalScheduler.h"

#define CONFIG_FILE "resources/config.txt"

void printHeader();
void printCommand();

namespace {
    // trims leading/trailing whitespace
    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
 
    // true if the whole string is a non-negative base-10 integer.
    bool isNumericString(const std::string& s) {
        if (s.empty()) return false;
        return std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isdigit(c); });
    }
 
    // splits a semicolon-separated instruction blob into trimmed instructions
    std::vector<std::string> splitInstructions(const std::string& blob) {
        std::vector<std::string> result;
        std::stringstream ss(blob);
        std::string token;
        while (std::getline(ss, token, ';')) {
            std::string trimmed = trim(token);
            if (!trimmed.empty()) {
                result.push_back(trimmed);
            }
        }
        return result;
    }
 
}


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

void MainConsole::handleCommand(const std::string& input) {
    std::string command;
    std::string args[3];   // args[0]=flag, args[1]=process name, args[2]=memory size (for -s/-c)
    std::stringstream ss(input);

    ss >> command >> args[0] >> args[1] >> args[2];

    if (command != "initialize" && command != "exit" && command != "clear" && !isConfigInitialized()) {
        std::cout << "Unknown command. Please run 'initialize' first.\n";
        return;
    }

    //printCommand(command, args);
    if (command == "initialize") {
        if (isConfigInitialized()) {
            std::cout << "Already initialized.\n";
        }
        else {
            Config temp;
            if (loadConfig(CONFIG_FILE, temp) && validateConfig(temp)) {
                getConfig() = temp;
                initializeConfig();
                GlobalScheduler::init(temp);
                std::cout << "Config initialized successfully.\n";
            }
            else {
                std::cout << "Config initialization failed. Please check " << CONFIG_FILE << ".\n";
            }
        }
    }
    else if (command == "exit") {
        if (GlobalScheduler::getInstance() != nullptr) {
            GlobalScheduler::getInstance()->stop();
        }
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
            else if (args[2] == "" || !isNumericString(args[2])) {
                std::cout << "invalid memory allocation\n";
            }
            else {
                unsigned long long memSize;
                try {
                    memSize = std::stoull(args[2]);
                } catch (...) {
                    std::cout << "invalid memory allocation\n";
                    return;
                }
                if (!isValidMemorySize(memSize)) {
                    std::cout << "invalid memory allocation\n";
                }
                else if (GlobalScheduler::getInstance() != nullptr) {
                    auto process = GlobalScheduler::getInstance()->findProcess(args[1]);
                    if (process == nullptr) {
                        // NOTE for Member 3: for createUniqueProcess 
                        // Process(pid, name, memoryRequired) is constructed with memSize
                        // already validated against [2^6, 2^16] above
                        process = GlobalScheduler::getInstance()->createUniqueProcess(args[1], memSize, true);
                    }
                    auto screen = std::make_shared<BaseScreen>(process, args[1]);
                    ConsoleManager::getInstance()->registerScreen(screen);
                    ConsoleManager::getInstance()->switchToScreen(screen->getName());
                }
                else {
                    std::cout << "Scheduler is not initialized. Please run 'initialize' first.\n";
                }
            }
        }
        else if (args[0] == "-r") {
            if (GlobalScheduler::getInstance() != nullptr) {
                if (args[1] == "") {
                    std::cout << "Please enter the process name.\n";
                }
                else {
                    auto process = GlobalScheduler::getInstance()->findProcess(args[1]);
                    if (process != nullptr) {
                        // NOTE for Member 2: once Process tracks a memory-
                        // access-violation shutdown (bool + timestamp +
                        // faulting address), branch here instead and print:
                        // "Process <name> shut down due to memory access
                        //  violation error that occurred at <HH:MM:SS>.
                        //  <Hex memory address> invalid."

                        // Check for access violation first (even if process state is FINISHED)
                        if (process->hasAccessViolation()) {
                            std::cout << process->getAccessViolationMessage() << "\n";
                        }
                        // Otherwise open screen if still active
                        else if (process->getState() != ProcessState::FINISHED) {
                            OSThread::sleep(100);
                            auto screen = std::make_shared<BaseScreen>(process, args[1]);
                            ConsoleManager::getInstance()->registerScreen(screen);
                            ConsoleManager::getInstance()->switchToScreen(screen->getName());
                        }
                        else {
                            std::cout << "Process " << args[1] << " has finished execution.\n";
                        }
                    }
                    else {
                        std::cout << "Process " << args[1] << " not found.\n";
                    }
                }
            }
            else{
                std::cout << "Scheduler is not initialized. Please run 'initialize' first.\n";
            }
        }
        else if (args[0] == "-c") {
            // since instructions are quoted and semicolon-separated 
            size_t firstQuote = input.find('"');
            size_t lastQuote = input.rfind('"');

            if (args[1] == "") {
                std::cout << "Please enter the process name.\n";
                return;
            }
            
            // Missing or malformed memory size
            if (args[2] == "" || !isNumericString(args[2])) {
                std::cout << "invalid memory allocation\n";
                return;
            }
            
            // Missing quotes / malformed instruction string
            if (firstQuote == std::string::npos || lastQuote == std::string::npos || lastQuote <= firstQuote)
            {
                std::cout << "invalid command\n";
                return;
            }

            if (args[1] == "" || args[2] == "" ||
                firstQuote == std::string::npos || lastQuote == std::string::npos ||
                lastQuote <= firstQuote) {
                std::cout << "invalid command\n";
                return;
            }
            else if (!isNumericString(args[2])) {
                std::cout << "invalid memory allocation\n";
                return;
            }
            else {
                unsigned long long memSize;
                try {
                    memSize = std::stoull(args[2]); 
                } catch (...) {
                    std::cout << "invalid memory allocation\n";
                    return;
                }
                if (!isValidMemorySize(memSize)) {
                    std::cout << "invalid memory allocation\n";
                    return;
                }
                else {
                    std::string instructionBlob = input.substr(firstQuote + 1, lastQuote - firstQuote - 1);
                    std::vector<std::string> instructions = splitInstructions(instructionBlob);

                    // sends a string of 1 – 50 instructions to be executed by the specified process. 
                    // Throws “invalid command” if the instruction size is not met.
                   if (instructions.empty() || instructions.size() > 50)
                   {
                    std::cout << "invalid command\n";
                    return;
                }

                    else if (GlobalScheduler::getInstance() == nullptr) {
                        std::cout << "Scheduler is not initialized. Please run 'initialize' first.\n";
                        return;
                    }
                    else {
                        auto process = GlobalScheduler::getInstance()->findProcess(args[1]);
                        if (process != nullptr) {
                            std::cout << "Process " << args[1] << " already exists.\n";
                            return;
                        }
                        else {
                            // NOTE for Member 3: same memory-size-aware creation entry point as "screen -s" above
                            //process = GlobalScheduler::getInstance()->createUniqueProcess(args[1], memSize);

                            // NOTE for Member 2: Process needs a method (e.g.
                            // loadUserDefinedInstructions(const std::vector<std::string>&))
                            // that turns each validated instruction string
                            // here into the matching ICommand
                            // (PrintCommand/DeclareCommand/MathCommand/
                            // SleepCommand/ForCommand/ReadCommand/
                            // WriteCommand) and appends it via addCommand(),
                            // instead of the random generateCommandBlock()
                            // path used by Process::initializeCommands().
                            // `instructions` already holds the validated,
                            // semicolon-split, trimmed instruction list.
                            // process->loadUserDefinedInstructions(instructions);

                            // Create process with user-defined memory size
                            process = GlobalScheduler::getInstance()->createUniqueProcess(args[1], memSize, false);

                            // Load user instructions into process
                            process->loadUserDefinedInstructions(instructions);

                            auto screen = std::make_shared<BaseScreen>(process, args[1]);
                            ConsoleManager::getInstance()->registerScreen(screen);
                            ConsoleManager::getInstance()->switchToScreen(screen->getName());
                        }
                    }
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
            if (GlobalScheduler::getInstance()->hasStarted())
                std::cout << "Scheduler is already running\n";
            else {
                GlobalScheduler::getInstance()->setGenerating(true);
                if (GlobalScheduler::getInstance()->hasStarted())
                    std::cout << "Scheduler started.\n";
                else std::cout << "Error starting scheduler. \n";
            }
        }
    }
    else if (command == "scheduler-stop") {
        if (GlobalScheduler::getInstance() == nullptr) {
            std::cout << "Config not initialized yet.\n";
        }
        else
        {
            if (!GlobalScheduler::getInstance()->hasStarted())
                std::cout << "Scheduler already stopped\n";
            else {
                GlobalScheduler::getInstance()->setGenerating(false);
                if (!GlobalScheduler::getInstance()->hasStarted())
                    std::cout << "Scheduler stopped. \n";
                else std::cout << "Error stopping scheduler. \n";
            }
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
                << "(" << p->getCreatedTime() << ")    "
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
                << "(" << p->getCreatedTime() << ")    "
                << std::setw(12) << "Finished"
                << p->getCommandCounter() << " / " << p->getLinesOfCode() << "\n";
        }
    }
    else std::cout << "No finished processes\n";
    
    std::cout << "--------------------------------------------------\n";
}
