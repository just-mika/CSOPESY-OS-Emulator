#include <iostream>
#include "BaseScreen.h"
#include "ConsoleManager.h"
#include <string>

BaseScreen::BaseScreen(std::shared_ptr<Process> process, std::string processName) : AConsole(processName)
{
	this->attachedProcess = process;
}
void BaseScreen::onEnabled(){}

void BaseScreen::process(){
	if (!(this->refreshed)){
		this->refreshed = true;
		this->printProcessInfo();
	}

	std::cout << "Enter command: ";

	std::string command;
	std::getline(std::cin, command);

	if (command == "process-smi")
		this->printProcessInfo();
	else if (command == "exit")
	{
		ConsoleManager::getInstance()->returnToPreviousConsole();
		ConsoleManager::getInstance()->unregisterScreen(this->name);
	}
		
}
void BaseScreen::display(){}

void printProcessInfo(){}