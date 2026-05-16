#include "cli.h"
#include "system_state.h"
#include "initializer.h"
#include <iostream>
#include <string>
#include <sstream>

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
        //placeholder
        std::cout << "screen stuff\n";
    }
    else if (command == "scheduler-start") {
        //placeholder
        std::cout << "Scheduler Started\n";
    }
    else if (command == "scheduler-stop") {
        //placeholder
        std::cout << "Scheduler Stopped\n";
    }
    else if (command == "report-util") {
        //placeholder
        std::cout << "report generated\n";
    }
    else std::cout << "Unknown command\n";
}