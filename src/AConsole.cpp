#include "AConsole.h"

AConsole::AConsole(std::string name)
{
	this->name = name;
}

std::string AConsole::getName()
{
    return this->name;
}