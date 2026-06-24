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
	: ICommand(pid, CommandType::PRINT), toPrint(toPrint)
{
}

void PrintCommand::execute()
{
	auto process = GlobalScheduler::getInstance()->findProcess(pid);
	if (!process) return;

	std::string finalOutput = this->toPrint;

	if (finalOutput.empty() && this->varName.empty()) {
		finalOutput = "Hello world from " + process->getName() + "!"; // 
	}

	if (!this->varName.empty()) {
		PrimitiveValue liveVariable = process->getSymbolTable().getVariable(this->varName);

		if (!std::holds_alternative<std::monostate>(liveVariable)) {
			finalOutput += convertPrimitiveToString(liveVariable);
		}
	}

	// For debugging purposes only
	FileLogger::logCommandExecution(process->getName(), process->getCPUCoreID(), finalOutput);
}

std::string convertPrimitiveToString(const PrimitiveValue& variantVal)
{
	return std::visit([](auto&& arg) -> std::string {
		using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same_v<T, uint16_t>) {
			return std::to_string(arg); // Converts uint16_t to string
		}
		else if constexpr (std::is_same_v<T, float>) {
			return std::to_string(arg); // Converts float to string
		}
		else if constexpr (std::is_same_v<T, char>) {
			return std::string(1, arg); // Converts a single char to string
		}

		return "";
		}, variantVal);
}