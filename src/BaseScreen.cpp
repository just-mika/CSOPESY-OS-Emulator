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
		this->display();
	}


	std::cout << "root:\\> ";

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
void BaseScreen::display(){
	std::cout << "Process name: " << attachedProcess->getName() << "\n";
	std::cout << "ID: " << attachedProcess->getPID() << "\n\n";
	std::cout << "Current instruction line: " << attachedProcess->getCommandCounter() << "\n";
	std::cout << "Lines of code: " << attachedProcess->getLinesOfCode() << "\n\n";
}

//Prints info about the process
// Attributes needed: processName, execDT, core 
// just format and print the info as is
void BaseScreen::printProcessInfo() const{
	std::cout << "Process name: " << attachedProcess->getName() << "\n";
	std::cout << "ID: " << attachedProcess->getPID() << "\n\n";
	std::cout << "Logs:\n";


	if (attachedProcess->isFinished()) {
		std::cout << "Finished!\n\n";
	}
	else {
		std::cout << "Current instruction line: " << attachedProcess->getCommandCounter() << "\n";
		std::cout << "Lines of code: " << attachedProcess->getLinesOfCode() << "\n\n";
	}
}

//prints the current instruction of attached process
void BaseScreen::printCurrentLine() const
{

}