#include "Process.h"

Process::Process(int pid, std::string name)
{
	this->pID = pid;
	this->name = name;
}

void Process::addCommand(std::shared_ptr<ICommand> command)
{
	
}

void Process::moveToNextLine()
{
	
}

void Process::nextInstruction()
{
	
}

bool Process::isFinished() const
{ 
	if (currentState == FINISHED)
		return true;
	return false;
}

int Process::getPID() const
{
	return pID;
}

ProcessState Process::getState() const
{
	return currentState;
}

std::string Process::getName() const
{
	return name;
}

SymbolTable& Process::getSymbolTable()
{
	return symbolTable;
}