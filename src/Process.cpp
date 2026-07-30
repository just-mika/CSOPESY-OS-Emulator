#include "Process.h"
#include "PrintCommand.h"
#include "DeclareCommand.h"
#include "MathCommand.h"
#include "SleepCommand.h"
#include "FileLogger.h"
#include "ForCommand.h"
#include "FlatMemoryAllocator.h"
#include "ReadCommand.h"
#include "WriteCommand.h"
#include "Windows.h"
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdlib>

Process::Process(int pid, std::string name, size_t memoryRequired)
{
	this->pID = pid;
	this->name = name;
	this->commandCounter = 0;
	this->memoryRequired = memoryRequired;
	this->currentState = READY;
	this->cpuCoreID = -1; // has not been assigned to a core yet
	this->creationTime = std::time(nullptr);
	this->printLogs = std::make_shared<std::vector<std::string>>();
	this->virtualMemory.resize(memoryRequired, 0); // Allocate virtual memory space
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

static std::vector<std::shared_ptr<ICommand>> demoCase(
	int pID,
	std::string procName,
	int numCmds,
	int currentDepth)
{
	std::vector<std::shared_ptr<ICommand>> block;

	// Initialize variables
	block.push_back(std::make_shared<DeclareCommand>(pID, "one", 1));
	block.push_back(std::make_shared<DeclareCommand>(pID, "x", 0));
	block.push_back(std::make_shared<DeclareCommand>(pID, "y", 0));
	block.push_back(std::make_shared<DeclareCommand>(pID, "z", 0));

	std::vector<std::shared_ptr<ICommand>> loopBlock;

	loopBlock.push_back(std::make_shared<MathCommand>(
		pID, "x", "x", "one", CommandType::ADD));
	loopBlock.push_back(std::make_shared<PrintCommand>(
		pID, "value from x: ", "x"));

	loopBlock.push_back(std::make_shared<MathCommand>(
		pID, "y", "y", "one", CommandType::ADD));
	loopBlock.push_back(std::make_shared<PrintCommand>(
		pID, "value from y: ", "y"));

	loopBlock.push_back(std::make_shared<MathCommand>(
		pID, "z", "z", "one", CommandType::ADD));
	loopBlock.push_back(std::make_shared<PrintCommand>(
		pID, "value from z: ", "z"));

	block.push_back(std::make_shared<ForCommand>(pID, loopBlock, 100));

	return block;
} 
void Process::initializeCommands(int limit)
{
	//comment this in final submit
	//FileLogger::initializeProcessFile(this->name);

	// Call helper function to create a list of commands
	std::vector<std::shared_ptr<ICommand>> initialCommands = generateCommandBlock(this->pID, this->name, limit, 1); // Add 1 for command loop depth
		//demoCase(this->pID, this->name, limit, 1);
		//

	// Add them all to the process
	for (const auto& cmd : initialCommands) {
		this->addCommand(cmd);
	}
}

void Process::loadUserDefinedInstructions(const std::vector<std::string>& instructions)
{
	for (const auto& line : instructions)
	{
		std::stringstream ss(line); // This variable gets line then splits it into tokens
		std::string op; // This variable gets the first token of the line, which is the operation type (e.g., DECLARE, READ, WRITE, ADD, SUBTRACT, SLEEP, PRINT)
		ss >> op; // Extract the operation type from the line

		if (op == "DECLARE")
		{
			std::string varName;
			uint16_t defaultValue = 0;
			ss >> varName >> defaultValue;
			addCommand(std::make_shared<DeclareCommand>(pID, varName, defaultValue));
		}
		else if (op == "READ")
		{
			std::string targetVar;
			std::string addrStr;
			ss >> targetVar >> addrStr;

			uint32_t memoryAddress = std::stoul(addrStr, nullptr, 16);
			addCommand(std::make_shared<ReadCommand>(pID, targetVar, memoryAddress));
		}
		else if (op == "WRITE")
		{
			std::string addrStr;
			std::string valueOrVar;
			ss >> addrStr >> valueOrVar;

			uint32_t memoryAddress = std::stoul(addrStr, nullptr, 16);

			// If valueOrVar starts with a digit, parse as literal integer; otherwise treat as variable name
			if (!valueOrVar.empty() && (std::isdigit(valueOrVar[0]) || valueOrVar[0] == '-'))
			{
				uint16_t literalVal = static_cast<uint16_t>(std::stoul(valueOrVar));
				addCommand(std::make_shared<WriteCommand>(pID, memoryAddress, literalVal));
			}
			else
			{
				addCommand(std::make_shared<WriteCommand>(pID, memoryAddress, valueOrVar));
			}
		}
		else if (op == "ADD" || op == "SUBTRACT")
		{
			std::string targetVar, var1, var2;
			ss >> targetVar >> var1 >> var2;
			CommandType cmdType = (op == "ADD") ? CommandType::ADD : CommandType::SUBTRACT;
			addCommand(std::make_shared<MathCommand>(pID, targetVar, var1, var2, cmdType));
		}
		else if (op == "SLEEP")
		{
			int ticks = 0;
			ss >> ticks;
			addCommand(std::make_shared<SleepCommand>(pID, static_cast<uint8_t>(ticks)));
		}
		else if (op.rfind("PRINT", 0) == 0)
		{
			// Simple or formatted PRINT instruction
			addCommand(std::make_shared<PrintCommand>(pID));
		}
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

		/** ADDING A FOR LOOP COMMAND
		 *   This converts a tree-like nested loop structure into a flat, sequential array to be added in the commandList
		 *   while preserving the original code layout.
		 *   
		 *   This allows 'nextInstruction()' to know exactly where a loop starts, where it ends,
		 *   and where to jump, without ever duplicating lines or expanding iterations in memory.
		 */

		if (command->getCommandType() == CommandType::FOR) {
			auto forCmd = std::dynamic_pointer_cast<ForCommand>(command);

			int forIndex = commandList.size(); //capture current index of the for loop.
			commandList.push_back(forCmd); // Add the FOR header itself into the sequence

			// The loop body starts immediately at the next available index
			int bodyStart = commandList.size();

			// Recurse through the loop body to flatten its inner instructions
			for (const auto& inner : forCmd->loopInstructions) {
				addCommand(inner);
			}

			// Capture the index of last instruction belonging to this loop's body
			int bodyEnd = commandList.size() - 1;

			// Save the indexes of start and end instructions inside the ForCommand to be added.
			forCmd->setBodyRange(bodyStart, bodyEnd);
		}
		else {
			// Base commands (PRINT, MATH, SLEEP, DECLARE) are pushed sequentially into the layout
			commandList.push_back(command);
		}
	}
}
void Process::saveLog(std::string printedString) {
	time_t nowT = std::time(nullptr);
	std::string toSave = "(" + formatTime(nowT) + ")  ";
	toSave += "Core: " + std::to_string(cpuCoreID) + " " + printedString;
	printLogs->push_back(toSave);
}

std::shared_ptr<std::vector<std::string>> Process::getPrintLogs() const {
	return printLogs;
}

void Process::nextInstruction() {
	if (isFinished() || commandList.empty()) {
		return;
	}
	if (commandCounter == 0) {
		execDT = std::chrono::system_clock::now();
		currentState = RUNNING;
	}

	if (commandCounter < commandList.size()) {
		auto& cmd = commandList[commandCounter];

		// PROCESSING A FOR LOOP COMMAND
		/*
		 *  Instead of executing nested FOR loops instantly in a single clock cycle (which would block the CPU
		 *  and break quantum preemption or SLEEP commands), this keeps instructions in a flat list and uses a
		 *  'loopStack' to track nested loops.
		 *
		 *  This design lets our scheduler safely pause, sleep, or preempt the process at any specific line
		 *  inside a possibly nested loop structure.
		*/

		if (cmd->getCommandType() == CommandType::FOR) {
			auto forCmd = std::dynamic_pointer_cast<ForCommand>(cmd);

			// Check if this FOR is already on the stack
			if (!loopStack.empty() && loopStack.top().forIndex == commandCounter) {
				loopStack.top().currentIteration++;

				// Case if loop has completed all its required repetitions
				if (loopStack.top().currentIteration >= forCmd->repeats) {
					loopStack.pop();                      // Remove this loop layer from stack
					commandCounter = forCmd->bodyEnd + 1; // Advance past the loop body entirely

					// If we just exited an inner loop but are still inside a parent loop body,
					// jump back to evaluate the parent loop header.
					if (!loopStack.empty() && commandCounter > loopStack.top().bodyEnd) {
						commandCounter = loopStack.top().forIndex;
					}
				}
				// Loop still has repetitions remaining -> jump back to start of its body
				else {
					commandCounter = forCmd->bodyStart;
				}
			}
			// First time hitting this FOR block -> Register loop parameters onto our execution stack
			else {
				loopStack.push({
					commandCounter,
					forCmd->bodyStart,
					forCmd->bodyEnd,
					forCmd->repeats, 0 });
				commandCounter = forCmd->bodyStart; // Step cleanly into the first line of the loop body
			}
		}
		else {
			cmd->execute();

			// Stop process execution if access violation occurred during command execution
			if (accessViolation) {
				return;
			}

			if (cmd->getCommandType() == CommandType::PRINT) {
				auto printCmd = std::dynamic_pointer_cast<PrintCommand>(cmd);
				if (printCmd) {
					saveLog(printCmd->getToPrint());
				}
			}
			commandCounter++;

			// Check if we just walked past the end line of our active loop body.
			if (!loopStack.empty() && commandCounter > loopStack.top().bodyEnd) {
				commandCounter = loopStack.top().forIndex;
			}
		}
	}

	// Only finish if the list is exhausted AND no loops are left open on the stack
	if (commandCounter >= (int)commandList.size() && loopStack.empty()) {
		currentState = FINISHED;
	}
}

bool Process::isValidAddress(uint32_t addr) const {
	// Ensure address is within bounds and has at least 2 bytes available for uint16
	return (addr < memoryRequired) && (memoryRequired - addr >= 2);
}

void Process::triggerAccessViolation(uint32_t addr) {
	this->accessViolation = true;
	this->invalidAddress = addr;
	this->currentState = FINISHED;

	time_t now = std::time(nullptr);
	std::tm tm_struct;
#ifdef _WIN32
	localtime_s(&tm_struct, &now);
#else
	localtime_r(&now, &tm_struct);
#endif
	std::stringstream ss;
	ss << std::put_time(&tm_struct, "%H:%M:%S");
	this->violationTimestamp = ss.str();
}

bool Process::hasAccessViolation() const {
	return accessViolation;
}

std::string Process::getAccessViolationMessage() const {
	std::stringstream ss;
	ss << "Process " << name << " shut down due to memory access violation error that occurred at "
		<< violationTimestamp << ". 0x" << std::hex << std::uppercase << invalidAddress << " invalid.";
	return ss.str();
}

uint16_t Process::readMemory(uint32_t addr) {
	if (!isValidAddress(addr)) {
		triggerAccessViolation(addr);
		return 0; // Return 0 on uninitialized/invalid addresses
	}
	uint16_t value = static_cast<uint16_t>(virtualMemory[addr]) |
		(static_cast<uint16_t>(virtualMemory[addr + 1]) << 8);
	return value;
}

void Process::writeMemory(uint32_t addr, uint16_t value) {
	if (!isValidAddress(addr)) {
		triggerAccessViolation(addr);
		return;
	}
	virtualMemory[addr] = static_cast<uint8_t>(value & 0xFF);
	virtualMemory[addr + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
}

size_t Process::getMemoryRequired() const {
    return this->memoryRequired;
}

void* Process::getMemoryAddress() const {
	return this->memoryAddress;
}

void Process::setMemoryAddress(void* ptr) {
	this->memoryAddress = ptr;
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
void Process::setState(ProcessState state) {
	this->currentState = state;
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
