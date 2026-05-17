#include "system_state.h"
#include "screen.h"

#include <iostream>
#include <string>

void screenCommand(SystemState& state, std::string arg, std::string processName) {
	if (!state.initialized)
		std::cout << "Config not yet initialized\n";
	else {
		if (arg != "-s" && arg != "-ls" && arg != "-r")
			std::cout << arg << " is an invalid argument\n";
		else {
			//placeholder
			std::cout << "Viewing Process " << processName << std::endl;

			//put logic here
		}
	}
}