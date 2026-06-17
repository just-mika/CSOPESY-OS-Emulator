#include "Process.h"
#include <iomanip>
#include <sstream>

Process::Process(int pid, std::string name)
{
    this->pID = pid;
    this->name = name;
    
    this->creationTime = std::time(nullptr);
}

std::string Process::getFormattedCreationTime() const 
{
    std::tm tm_struct;
    
    // Thread-safe local time extraction (for both Windows and Mac)
	// Reference: https://cplusplus.com/forum/general/189594/
#ifdef _WIN32
    localtime_s(&tm_struct, &this->creationTime);
#else
    localtime_r(&this->creationTime, &tm_struct);
#endif

    std::stringstream ss;
    ss << std::put_time(&tm_struct, "%m/%d/%Y %I:%M:%S%p");
    return ss.str();
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

int Process::getCommandCounter() const
{
    return commandCounter;
}

int Process::getLinesOfCode() const
{
    return commandList.size();
}

int Process::getCPUCoreID() const
{
    return cpuCoreID;
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
