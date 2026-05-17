#include "system_state.h"
#include "scheduler.h"

#include <iostream>
#include <string>

void startScheduler(SystemState& state) {
	if (!state.initialized)
		std::cout << "Config not yet initialized\n";
	else {
		//placeholder
		std::cout << "Scheduler Started\n";

		//put logic here
	}
}
void stopScheduler(SystemState& state) {
	if (!state.initialized)
		std::cout << "Config not yet initialized\n";
	else {
		//placeholder
		std::cout << "Scheduler Stopped\n";

		//put logic here
	}
}