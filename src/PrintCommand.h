#pragma once
#include <string>

#include "ICommand.h"

class PrintCommand : public ICommand
{
public:
	PrintCommand(int pid, std::string processName, std::string& toPrint);
	void execute() override;
	void setCoreID(int coreID);
private:
	std::string toPrint;
	std::string processName;
	int activeCoreID = -1;
};