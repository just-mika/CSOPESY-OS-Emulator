#include "Process.h"
#include "PrintCommand.h"
#include <iomanip>
#include <sstream>
#include <fstream>

#include "DeclareCommand.h"
#include "FileLogger.h"
#include "Windows.h"

Process::Process(int pid, std::string name)
{
	this->pID = pid;
	this->name = name;
	this->commandCounter = 0;
	this->currentState = READY;
	this->cpuCoreID = -1; // has not been assigned to a core yet
	this->creationTime = std::time(nullptr);
}

void Process::initializeCommands(int limit)
{
	FileLogger::initializeProcessFile(this->name);

	for (int i = 1; i <= limit; ++i) {
		std::string varName = "v" + std::to_string(i);
		uint16_t randomDefaultValue = static_cast<uint16_t>(rand() % 65536);
		std::shared_ptr<ICommand> generatedCmd = std::make_shared<DeclareCommand>(
			this->pID,
			varName,
			randomDefaultValue
		);
		Sleep(1);
		this->addCommand(generatedCmd);
	}
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
	if (command != nullptr) {
		commandList.push_back(command);
	}
}

void Process::moveToNextLine()
{
	commandCounter++;
	if (commandCounter >= static_cast<int>(commandList.size())) {
		currentState = FINISHED;
	}
}

void Process::nextInstruction()
{
	if (isFinished() || commandList.empty()) {
		return;
	}
	if (commandCounter == 0) {
		execDT = std::chrono::system_clock::now();
		currentState = RUNNING;
	}
	if (commandCounter < static_cast<int>(commandList.size())) {
		std::shared_ptr<ICommand> cmd = std::dynamic_pointer_cast<ICommand>(commandList[commandCounter]); // Current command being executed
		commandList[commandCounter]->execute();
		moveToNextLine();
	}
}

void Process::incrementCyclesInCPU()
{
	cyclesInCPU++;
}
void Process::resetCyclesInCPU()
{
	cyclesInCPU = 0;
}

int Process::getCyclesInCPU() const
{
	return cyclesInCPU;
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
	return currentState == FINISHED;
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

void Process::setCPUCoreID(int coreID)
{
	this->cpuCoreID = coreID;
}

void Process::pauseProcess() {
	if (currentState == RUNNING)
		currentState = READY;
}

void Process::setRemainingSleepTicks(int ticks) {
	this->remainingSleepTicks = ticks;
}

int Process::getRemainingSleepTicks() const {
	return remainingSleepTicks;
}

void Process::sleepProcess() {
	if (currentState == RUNNING || currentState == READY) {
		currentState = WAITING;
	}
}