#pragma once
#include <string>

#include "ICommand.h"
#include "SymbolTable.h"

class PrintCommand : public ICommand
{
public:
	PrintCommand(int pid, std::string toPrint, std::string varName);
	PrintCommand(int pid);
	PrintCommand(int pid, std::string toPrint);
	void execute() override;
	std::string getToPrint();
private:
	std::string toPrint;
	std::string varName;
	std::string processStringToPrint();
};
