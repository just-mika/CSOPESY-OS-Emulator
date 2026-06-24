#include "ICommand.h"

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