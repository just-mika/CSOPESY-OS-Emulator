// main.cpp : Entry point of the CSOPESY OS Emulator.
//
// Description:
//   Initializes the system state and runs the main command loop.
//   Handles user input and delegates commands to the CLI handler.
//

#include "cli.h"
#include "system_state.h"
#include <iostream>
#include <string>
#include <sstream>


int main()
{
    std::string input;
    SystemState state; //Current state of the system

    std::cout << "*==========================================*";
    std::cout << "\nCSOPESY OS Emulator\n\n";

    std::cout << "Developed by S09 Group 7\n";
    std::cout << "Almoradie, Nicole\n";
    std::cout << "Amon, Mikaela\n";
    std::cout << "Filipino, Eunice\n";
    std::cout << "Wee, Justine\n";
    std::cout << "*==========================================*";

    while (state.sysRunning) {
        std::cout << "\nroot:\\>";
        getline(std::cin, input);

        handleCommand(state, input);
    }

    return 0;
}
