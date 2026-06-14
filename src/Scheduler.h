#pragma once
#include "System.h"
#include <string>

class Scheduler {
private:
	std::string algorithm;

	//add variables 
public:
	Scheduler(std::string algo);
	void start(bool initialized);
	void stop(bool initialized);
	//add methods
};