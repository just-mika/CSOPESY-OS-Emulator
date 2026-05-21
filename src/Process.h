#pragma once
#include <string>
#include <vector>
#include <chrono>

#include "Command.h"

enum class State
{
	READY,
	RUNNING,
	FINISHED
};

class Process
{
	private:
		int pID;                            //process ID
		std::string processName;            // process_name
		std::vector<Command> instructions;  //list of instructions
		State state = State::READY;         //current state of process
		int core = 0;                       //assigned core of process
		int currentLine = 0;                //current instruction being executed
		std::chrono::system_clock::time_point execDT; //date and time the process starts executing

	public:
		Process(int id, const std::string& name, const std::vector<Command>& instructions);

		void executeCurrentInstruction();
		void nextInstruction();

		void printProcess(); //Prints info about the process
		// Attributes needed: processName, execDT, core 
		// just format and print the info as is

		void printCurrentLine(); //prints the current instruction
		// Attributes needed: currentLine, instructions
		// idea: store the index/pID current instruction being run, retrieve it from list of instructions, and print it
};

/*
int pid = 0; //example pid
std::unordered_map<int, Process> processes; // unordered map traversal in O(N)
Process& p1 = processes.at(pid); // retrieve process in O(1)*/