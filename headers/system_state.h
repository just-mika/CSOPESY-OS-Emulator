// system_state.h : Defines global system state for the emulator.
//
// Description:
//   Contains runtime flags and configuration used across the system
//   such as initialization state, scheduler status, and config data.
//

#pragma once
#include "config.h"

struct SystemState {
	bool initialized = false;
	bool schedulerRunning = false;
	bool sysRunning = true;
	Config config;
	//add more variables 
};