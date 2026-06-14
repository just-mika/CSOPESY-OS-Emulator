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
void MainConsole::handleCommand(const std::string& input) {
    std::string command;
    std::string args[2];
    std::stringstream ss(input);

    ss >> command >> args[0] >> args[1];

    //printCommand(command, args);
    if (command == "initialize") { 
        if (GlobalScheduler::getInstance() != nullptr) {
            std::cout << "Config lready initialized.\n";
        }
        else {
            Config temp;
            if (loadConfig(CONFIG_FILE, temp) && validateConfig(temp)) {
                GlobalScheduler::initialize(temp);
				std::cout << "Config initialized successfully.\n";

                GlobalScheduler::getInstance()->printConfig();
            }
        }
    }
    else if (command == "exit") {
        ConsoleManager::getInstance()->exitApplication();
    }
    else if (command == "clear") {
        system("cls");
        display();
    }
    else if (command == "screen") {
        if (args[1] == "-ls")
			std::cout << "Running processes: \n";
        //std::cout << command << " command recognized. Doing something.\n";
    }
    else if (command == "scheduler-start") {
        std::cout << command << " command recognized. Doing something.\n";
    }
    else if (command == "scheduler-stop") {
        std::cout << command << " command recognized. Doing something.\n";
    }
    else if (command == "report-util") {
    	std::cout << command << " command recognized. Doing something.\n";
            //std::cout << "Report generated at C:<filepath>/output/csopesy-log.txt!\n";
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
