#include "ICommand.h"

#include "SymbolTable.h"

ICommand::ICommand(int pid, CommandType type)
{
	this->pid = pid;
	this->commandType = type;
}

CommandType ICommand::getCommandType()
{
	return commandType;
}

void ICommand::execute()
{
	
}

std::string ICommand::convertPrimitiveToString(const PrimitiveValue& variantVal)
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