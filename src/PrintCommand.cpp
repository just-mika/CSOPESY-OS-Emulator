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

PrintCommand::PrintCommand(int pid, std::string processName, std::string toPrint, std::string varName)
	: ICommand(pid, CommandType::PRINT), processName(processName), toPrint(toPrint), variableName(varName)
{
	if (this->toPrint.empty() && this->variableName.empty()) {
        this->toPrint = "Hello world from " + processName + "!";
    }
}

void PrintCommand::execute()
{
	std::string finalOutput = toPrint;

	if (!variableName.empty()) {
		auto process = GlobalScheduler::getInstance()->findProcess(processName);
		if (process) {
			// 1. Fetch the variant from the symbol table
			PrimitiveValue variantVal = process->getSymbolTable().getVariable(variableName);

			finalOutput += convertPrimitiveToString(variantVal);
		}
	}

	//for debugging purposes only.
	FileLogger::logCommandExecution(this->processName, this->activeCoreID, finalOutput);
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