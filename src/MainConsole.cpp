#include <iostream>
#include <string>
#include <sstream>

#include "MainConsole.h"

#include "ConsoleManager.h"
#include "System.h"

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

void MainConsole::handleCommand(const std::string& input) {
    std::string command;
    std::string args[2];
    std::stringstream ss(input);

    ss >> command >> args[0] >> args[1];

    //printCommand(command, args);
    if (command == "initialize") {
        //std::cout << command << " command recognized. Doing something.\n";
        System::getInstance()->initializeConfig();
    }
    else if (command == "exit") {
        ConsoleManager::getInstance()->exitApplication();
    }
    else if (command == "clear") {
        system("cls");
        display();
    }
    else if (command == "screen") {
        std::cout << command << " command recognized. Doing something.\n";
        //system.<method> 
    }
    else if (command == "scheduler-start") {
        std::cout << command << " command recognized. Doing something.\n";
        //system.startScheduler();
    }
    else if (command == "scheduler-stop") {
        std::cout << command << " command recognized. Doing something.\n";
        //system.stopScheduler();
    }
    else if (command == "report-util") {
       // if (system.generateReport())
    	std::cout << command << " command recognized. Doing something.\n";
            //std::cout << "Report generated at C:<filepath>/output/csopesy-log.txt!\n";
    }
    else std::cout << "Unknown command: " << input << std::endl;
}