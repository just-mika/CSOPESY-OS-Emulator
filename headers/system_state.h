#pragma once
#include "config.h"

struct SystemState {
	bool initialized = false;
	bool schedulerRunning = false;
	bool sysRunning = true;
	Config config;
};