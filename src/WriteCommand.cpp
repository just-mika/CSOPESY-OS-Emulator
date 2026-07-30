#include "WriteCommand.h"
#include "GlobalScheduler.h"
#include "Process.h"

WriteCommand::WriteCommand(int pid, uint32_t memoryAddress, uint16_t literalValue)
    : ICommand(pid, CommandType::WRITE), memoryAddress(memoryAddress), literalValue(literalValue), isFromVariable(false)
{
}

WriteCommand::WriteCommand(int pid, uint32_t memoryAddress, std::string sourceVar)
    : ICommand(pid, CommandType::WRITE), memoryAddress(memoryAddress), sourceVar(sourceVar), isFromVariable(true)
{
}

void WriteCommand::execute()
{
	// Find the process by PID
    std::shared_ptr<Process> process = GlobalScheduler::getInstance()->findProcess(pid);
    if (!process) return;

	uint16_t valToWrite = literalValue; // Default to literal value (ex. WRITE 0x2000 42, 42 is the literal value)

	if (isFromVariable) // If the value is to be taken from a variable
    {
        Symbol s = process->getSymbolTable().getVariable(sourceVar);
        if (std::holds_alternative<uint16_t>(s.value)) // Check if the variable is of type uint16_t
        {
            valToWrite = std::get<uint16_t>(s.value); // if the variable is of type uint16_t, get its value, then put  it to valToWrite
        }
    }

    // Write uint16 value to process memory space
    process->writeMemory(memoryAddress, valToWrite);
}
