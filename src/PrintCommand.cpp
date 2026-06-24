#include "PrintCommand.h"
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iostream>

#include "FileLogger.h"
#include "GlobalScheduler.h"
#include "SymbolTable.h"
std::string convertPrimitiveToString(const PrimitiveValue& variantVal);

PrintCommand::PrintCommand(int pid)
	: ICommand(pid, CommandType::PRINT)
{
}

PrintCommand::PrintCommand(int pid, std::string toPrint)
	: ICommand(pid, CommandType::PRINT), toPrint(toPrint)
{
}

PrintCommand::PrintCommand(int pid, std::string toPrint, std::string varName)
	: ICommand(pid, CommandType::PRINT), toPrint(toPrint), varName(varName)
{
}

void PrintCommand::execute()
{
	auto process = GlobalScheduler::getInstance()->findProcess(pid);
	if (!process) return;

	std::string finalOutput = this->toPrint;

	if (finalOutput.empty() && this->varName.empty()) {
		finalOutput = "Hello world from " + process->getName() + "!"; 
	}

	if (!this->varName.empty()) {
		PrimitiveValue liveVariable = process->getSymbolTable().getVariable(this->varName).value;

		if (!std::holds_alternative<std::monostate>(liveVariable)) {
			finalOutput += convertPrimitiveToString(liveVariable);
		}
		else {
			finalOutput += "[UNINITIALIZED/NULL VAR]";
		}
	}

	// For debugging purposes only
	FileLogger::logCommandExecution(process->getName(), process->getCPUCoreID(), finalOutput);
}

