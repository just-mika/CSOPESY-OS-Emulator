#include <iostream>
#include <string>
#include <sstream>

#include "Console.h"
#include "System.h"

Console::Console(System& system) : system(system) {}


void Console::run() {
	std::string input;

    std::cout << "*==========================================*";
    std::cout << "\nCSOPESY OS Emulator\n\n";

    std::cout << "Developed by S09 Group 7\n";
    std::cout << "Almoradie, Nicole\n";
    std::cout << "Amon, Mikaela\n";
    std::cout << "Filipino, Eunice\n";
    std::cout << "Wee, Justine\n";
    std::cout << "*==========================================*";

	while (system.isRunning()) {
        std::cout << "\nEnter a command: ";
        getline(std::cin, input);

        handleCommand(input);
	}
};

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

void Console::handleCommand(const std::string& input) {
    std::string command;
    std::string args[2];
    std::stringstream ss(input);

    ss >> command >> args[0] >> args[1];

    //printCommand(command, args);
    if (command == "initialize") {
        system.initialize();
    }
    else if (command == "exit") {
        system.shutdown();
    }
    else if (command == "screen") {
        //placeholder
        printCommand(command, args);
        //idk how to implement this yet.
        //system.<method>
    }
    else if (command == "scheduler-start") {
        system.startScheduler();
    }
    else if (command == "scheduler-stop") {
        system.stopScheduler();
    }
    else if (command == "report-util") {
        if (system.generateReport())
            std::cout << "Report generated at C:<filepath>/output/csopesy-log.txt!\n";
    }
    else std::cout << "Unknown command: " << input << std::endl;
}