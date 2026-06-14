#pragma once
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
	CommandType getCommandType();
	virtual void execute();

protected:
	int pid;
	CommandType commandType;
};