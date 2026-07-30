#include "ReadCommand.h"
#include "GlobalScheduler.h"
#include "Process.h"

ReadCommand::ReadCommand(int pid, std::string targetVar, uint32_t memoryAddress)
    : ICommand(pid, CommandType::READ), targetVar(targetVar), memoryAddress(memoryAddress)
{
}

void ReadCommand::execute()
{
    std::shared_ptr<Process> process = GlobalScheduler::getInstance()->findProcess(pid);
    if (!process) return;

    // Read uint16 value from emulated memory space
    uint16_t value = process->readMemory(memoryAddress);

    // If an access violation happened during readMemory, return immediately
    if (process->hasAccessViolation()) return;

    // Store value into symbol table variable
    process->getSymbolTable().setVariable(targetVar, PrimitiveType::UINT16, value);
}
