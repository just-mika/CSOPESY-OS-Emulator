#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include "ICommand.h"
#include "SymbolTable.h"
#include <ctime>

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

	public:
		Process(int pid, std::string name);
		void addCommand(std::shared_ptr<ICommand> command);
		void moveToNextLine();
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

		// Setters
		void setCPUCoreID(int coreID);
		void setRemainingSleepTicks(int ticks);
};

/*
struct requirementFlags {
	bool requireFiles;
	int numFiles;
	bool requireMemory;
	int memoryRequired;
}
 */

