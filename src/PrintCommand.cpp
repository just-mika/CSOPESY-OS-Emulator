#include "PrintCommand.h"

PrintCommand::PrintCommand(int pid, std::string& toPrint) : ICommand(pid, CommandType::PRINT), toPrint(toPrint)
{
	
}

void PrintCommand::execute()
{
	
}
