#pragma once
#include "SymbolTable.h"

enum CommandType
{
	IO,
	PRINT,
	DECLARE,
	ADD,
	SUBTRACT,
	SLEEP,
	FOR
};

class ICommand
{
public:
	ICommand(int pid, CommandType type);
	std::string convertPrimitiveToString(const PrimitiveValue& variantVal);
	CommandType getCommandType();
	virtual void execute();
protected:
	int pid;
	CommandType commandType;
};