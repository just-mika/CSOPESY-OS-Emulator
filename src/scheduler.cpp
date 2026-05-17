#include "System.h"
#include "Scheduler.h"
#include <iostream>


void Scheduler::start(bool initialized) {
	 if (!initialized)
		 std::cout << "Config not yet initialized\n";
	 else {
		 //placeholder
		 std::cout << "Scheduler Started\n";

		 //put logic here
	 }
}

void Scheduler::stop(bool initialized) {
	if (!initialized)
		std::cout << "Config not yet initialized\n";
	else {
		//placeholder
		std::cout << "Scheduler Stopped\n";

		//put logic here
	}
}