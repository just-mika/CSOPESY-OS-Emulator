#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include "ICommand.h"
#include "SymbolTable.h"
#include <ctime>
#include <stack>

struct LoopFrame {
	int forIndex;
	int bodyStart;
	int bodyEnd;
	int repeats;
	int currentIteration;
};

enum ProcessState {
	READY,
	RUNNING,
	WAITING,
	FINISHED
};


class Process
{
	private:
		int pID;                            //process ID
		std::string name;            // process_name
		std::vector<std::shared_ptr<ICommand>> commandList;  //list of instructions
		ProcessState currentState = READY;         //current state of process
		int cpuCoreID = -1;                       //assigned core of process
		int commandCounter;             //current instruction being executed
		std::chrono::system_clock::time_point execDT; //date and time the process starts executing
		SymbolTable symbolTable;            //symbol table for the process
		std::time_t creationTime; 
		int remainingSleepTicks = 0;		// For sleep command, to track how many ticks are left for the process to sleep
		int cyclesInCPU = 0;
		std::shared_ptr<std::vector<std::string>> printLogs; 
		void saveLog(std::string printedString);
		uint32_t mapAddress(uint32_t addr) const;
		std::stack<LoopFrame> loopStack;

		// Memory management
		void* memoryAddress = nullptr;		// Pointer to the allocated memory block for this process (if any)
		size_t memoryRequired;
		bool accessViolation = false;
		uint32_t invalidAddress = 0;
		std::string violationTimestamp;
		 

	public:
		Process(int pid, std::string name, size_t memoryRequired);
		void addCommand(std::shared_ptr<ICommand> command);
		void nextInstruction();
		void initializeCommands(int limit);
		void pauseProcess();
		void sleepProcess();
		void incrementCyclesInCPU();
		void resetCyclesInCPU();
		int getCyclesInCPU() const;
		void decrementSleepTicks();

		// Getters
		bool isFinished() const;
		int getCommandCounter() const;
		int getLinesOfCode() const;
		int getPID() const;
		int getCPUCoreID() const;
		std::shared_ptr<std::vector<std::string>> getPrintLogs() const;
		ProcessState getState() const;
		std::string getName() const;
		std::string getCreatedTime() const;
		int getRemainingSleepTicks() const;
		SymbolTable& getSymbolTable();
		size_t getMemoryRequired() const;
		void* getMemoryAddress() const;

		// Setters
		void setCPUCoreID(int coreID);
		void setRemainingSleepTicks(int ticks);
		void setMemoryAddress(void* ptr);
		void setState(ProcessState state);

		bool isValidAddress(uint32_t addr) const;
		void triggerAccessViolation(uint32_t addr);
		bool hasAccessViolation() const;
		std::string getAccessViolationMessage() const;

		void loadUserDefinedInstructions(const std::vector<std::string>& instructions);
		uint16_t readMemory(uint32_t addr);					// Read a 16-bit value from the process's virtual memory at the specified address
		void writeMemory(uint32_t addr, uint16_t value);	// Write a 16-bit value to the process's virtual memory at the specified address

};


/*
struct requirementFlags {
	bool requireFiles;
	int numFiles;
	bool requireMemory;
	int memoryRequired;
}
 */

