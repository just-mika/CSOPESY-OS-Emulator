#include "Process.h"
#include "PrintCommand.h"
#include <iomanip>
#include <sstream>
#include <fstream>
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
	std::string fileName = this->name + ".txt";
	std::ofstream outFile(fileName, std::ios::out);
	if (outFile.is_open()) {
		outFile << "Process name: " << this->name << "\n";
		outFile << "Logs:\n\n";
		outFile.close();
	}

	for (int i = 1; i <= limit; ++i) {
		std::string printText = "Hello world from " + this->name + "!";
		std::shared_ptr<ICommand> printCmd = std::make_shared<PrintCommand>(this->pID, this->name, printText); // Pass the process
		this->addCommand(printCmd); // Add the command to the process's command list
		Sleep(1);
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
		std::shared_ptr<ICommand> printCmd = std::dynamic_pointer_cast<ICommand>(commandList[commandCounter]); // Current command being executed

		commandList[commandCounter]->execute();
		moveToNextLine();
	}
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