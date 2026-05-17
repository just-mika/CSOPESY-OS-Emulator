#include "system_state.h"
#include "report.h"

#include <string>
#include <iostream>
#include <fstream>

bool generateReport(SystemState &state) {
	if (!state.initialized) {
		std::cout << "Config not yet initialized\n";
		return false;
	}
	//insert logic here
	
	return true;
}