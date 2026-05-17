// cli.cpp : Command-line interface command parser and dispatcher.
//
// Description:
//   Parses user input from the terminal and executes corresponding
//   system commands such as initialize, scheduler control, and screen handling.
//

#include "cli.h"
#include "system_state.h"
#include "initializer.h"
#include "screen.h"
#include "scheduler.h"
#include "report.h"

#include <iostream>
#include <string>
#include <sstream>

void handleCommand(SystemState& state, std::string input) {
    std::string command;
    std::string args[2];
    std::stringstream ss(input);

    ss >> command >> args[0] >> args[1];

    //printCommand(command, args);
    if (command == "initialize") {
        initialize(state);
    }
    else if (command == "exit") {
        state.sysRunning = false;
    }
    else if (command == "screen") {
        screenCommand(state, args[0], args[1]);
    }
    else if (command == "scheduler-start") {
        startScheduler(state);
    }
    else if (command == "scheduler-stop") {
        stopScheduler(state);
    }
    else if (command == "report-util") {
        //placeholder
        if (generateReport(state))
            std::cout << "Report generated at C:<filepath>/csopesy-log.txt!\n";
    }
    else std::cout << "Unknown command\n";
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