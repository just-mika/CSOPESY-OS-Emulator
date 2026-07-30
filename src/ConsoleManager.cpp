#include "ConsoleManager.h"
#include "MainConsole.h"
#include <iostream>

ConsoleManager* ConsoleManager::sharedInstance = nullptr;
ConsoleManager* ConsoleManager::getInstance()
{
	return sharedInstance;
}

void ConsoleManager::initialize()
{
	sharedInstance = new ConsoleManager();
}

void ConsoleManager::destroy()
{
	delete sharedInstance;
}

void ConsoleManager::drawConsole() const
{
	if (this->currentConsole != nullptr)
	{
		this->currentConsole->display();
	}
	else
	{
		std::cerr << "No assigned console" << std::endl;
	}
}

void ConsoleManager::process() const
{
	if (this->currentConsole != nullptr)
	{
		this->currentConsole->process();
	}
	else
	{
		std::cerr << "No assigned console" << std::endl;
	}
}

void ConsoleManager::switchConsole(std::string consoleName)
{
	if (this->consoleTable.contains(consoleName))
	{
		system("cls");
		this->previousConsole = this->currentConsole;
		this->currentConsole = this->consoleTable[consoleName];
		this->currentConsole->onEnabled();
	}
	else
	{
		std::cerr << "Console " << consoleName << " not found" << std::endl;
	}
}

void ConsoleManager::registerScreen(std::shared_ptr<BaseScreen> screenRef)
{
	if (this->consoleTable.contains(screenRef->getName()))
	{
		std::cerr << "Screen name " << screenRef->getName() << "already exists." << std::endl;
		return;
	}

	this->consoleTable[screenRef->getName()] = screenRef;
}

void ConsoleManager::switchToScreen(std::string screenName)
{
	if (this->consoleTable.contains(screenName))
	{
		system("cls");
		this->previousConsole = this->currentConsole;
		this->currentConsole = this->consoleTable[screenName];
		this->currentConsole->onEnabled();
	}
	else
	{
		std::cerr << "Screen name " << screenName << " not found" << std::endl;
	}
}

void ConsoleManager::unregisterScreen(std::string screenName)
{
	if (this->consoleTable.contains(screenName))
	{
		this->consoleTable.erase(screenName);
	}	
	else
	{
		std::cerr << "Unable to unregister " << screenName << std::endl;
	}
}

void ConsoleManager::returnToPreviousConsole()
{
	if (this->currentConsole != nullptr)
	{
		system("cls");
		this->currentConsole = previousConsole;
		this->previousConsole = nullptr;
		this->currentConsole->onEnabled();
	}
	else
	{
		std::cerr << "No assigned console" << std::endl;
	}
}

void ConsoleManager::exitApplication()
{
	this->running = false;
}

bool ConsoleManager::isRunning() const
{
	return this->running;
}
ConsoleManager::ConsoleManager()
{
	this->running = true;

	//this->consoleHandle = GetStdHandle(STD_OUTPUT)HANDLE);

	const std::shared_ptr<MainConsole> mainConsole = std::make_shared<MainConsole>();

	this->consoleTable["MAIN_CONSOLE"] = mainConsole;

	this->switchConsole("MAIN_CONSOLE");
}