#include "Process.h"
#include "PrintCommand.h"
#include "DeclareCommand.h"
#include "MathCommand.h"
#include "SleepCommand.h"
#include "FileLogger.h"
#include "ForCommand.h"
#include "Windows.h"
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdlib>

Process::Process(int pid, std::string name)
{
	this->pID = pid;
	this->name = name;
	this->commandCounter = 0;
	this->currentState = READY;
	this->cpuCoreID = -1; // has not been assigned to a core yet
	this->creationTime = std::time(nullptr);
	this->printLogs = std::make_shared<std::vector<std::string>>();
}

// Helper function to generate a block of commands for a FOR loop, with depth control to prevent infinite nesting
static std::vector<std::shared_ptr<ICommand>> generateCommandBlock(int pID, std::string procName, int numCmds, int currentDepth)
{
	std::vector<std::shared_ptr<ICommand>> block;

	for (int i = 0; i < numCmds; ++i) {

		// If depth is 3 or more, restrict to cases 0-4 (No FOR loops). Otherwise, cases 0-5.
		int maxCommandType = (currentDepth >= 3) ? 5 : 6;
		int commandType = rand() % maxCommandType;

		std::shared_ptr<ICommand> generatedCmd = nullptr;

		std::string randomVar1 = "v" + std::to_string(rand() % 5);
		std::string randomVar2 = "v" + std::to_string(rand() % 5);
		uint16_t randomVal = static_cast<uint16_t>(rand() % 65536);

		switch (commandType) {
		case 0:
			generatedCmd = std::make_shared<PrintCommand>(pID);
			break;
		case 1:
			generatedCmd = std::make_shared<DeclareCommand>(pID, randomVar1, randomVal);
			break;
		case 2:
			generatedCmd = std::make_shared<MathCommand>(pID, randomVar1, randomVar2, randomVal, CommandType::ADD);
			break;
		case 3:
			generatedCmd = std::make_shared<MathCommand>(pID, randomVar1, randomVar2, randomVal, CommandType::SUBTRACT);
			break;
		case 4:
			generatedCmd = std::make_shared<SleepCommand>(pID, static_cast<uint8_t>((rand() % 5) + 1));
			break;
		case 5: {
			// FOR LOOP
			int innerCmdsAmount = (rand() % 3) + 1;
			int repeats = (rand() % 4) + 2;

			// Call the helper recursively to generate the inner block of commands, increasing the depth by 1	
			auto innerBlock = generateCommandBlock(pID, procName, innerCmdsAmount, currentDepth + 1);
			generatedCmd = std::make_shared<ForCommand>(pID, innerBlock, repeats);
			break;
		}
		}

		if (generatedCmd != nullptr) {
			block.push_back(generatedCmd);
		}
	}
	return block;
}

void Process::initializeCommands(int limit)
{
	//comment this in final submit
	FileLogger::initializeProcessFile(this->name);

	// Call helper function to create a list of commands
	std::vector<std::shared_ptr<ICommand>> initialCommands = generateCommandBlock(this->pID, this->name, limit, 1); // Add 1 for command loop depth

	// Add them all to the process
	for (const auto& cmd : initialCommands) {
		this->addCommand(cmd);
	}
}

std::string static formatTime(time_t timeToFormat) {
	std::tm tm_struct;
#ifdef _WIN32
	localtime_s(&tm_struct, &timeToFormat);
#else
	localtime_r(&timeToFormat, &tm_struct);
#endif
		std::stringstream ss;
		ss << std::put_time(&tm_struct, "%m/%d/%Y %I:%M:%S%p");
		return ss.str();
}

std::string Process::getCreatedTime() const {
	return formatTime(this->creationTime);
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

void Process::saveLog(std::string printedString) {
	time_t nowT = std::time(nullptr);
	std::string toSave = "(" + formatTime(nowT) + ")  ";
	toSave += "Core: " + std::to_string(cpuCoreID) + " " + printedString;
	printLogs->push_back(toSave);
}
std::shared_ptr<std::vector<std::string>> Process::getPrintLogs() const{
	return printLogs;
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
		if (cmd->getCommandType() == CommandType::PRINT) {
			auto printCmd = std::dynamic_pointer_cast<PrintCommand>(cmd);
			if (printCmd) {
				saveLog(printCmd->getToPrint());
			}
		}
		cmd->execute();
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

void Process::decrementSleepTicks() {
	if (remainingSleepTicks > 0) {
		remainingSleepTicks--;
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
