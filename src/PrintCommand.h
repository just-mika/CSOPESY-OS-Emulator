#pragma once
#include <string>

#include "ICommand.h"

class PrintCommand : public ICommand
{
public:
	PrintCommand(int pid, std::string processName, std::string toPrint, std::string varName = "");
	void execute() override;
private:
	std::string toPrint;
	std::string processName;
	std::string variableName;
};