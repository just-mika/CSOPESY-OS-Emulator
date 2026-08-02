#include "DeclareCommand.h"

#include "FileLogger.h"
#include "GlobalScheduler.h"
#include "PagedMemoryAllocator.h"
#include "Process.h"

DeclareCommand::DeclareCommand(int pid, std::string varName, uint16_t defaultValue)
    : ICommand(pid, CommandType::DECLARE), varName(varName), defaultValue(defaultValue)
{
}

void DeclareCommand::execute()
{
    std::shared_ptr<Process> process = GlobalScheduler::getInstance()->findProcess(pid);
    if (!process) return;

    auto* pageTable = static_cast<PageTable*>(process->getMemoryAddress());
    PagedMemoryAllocator::getInstance()->ensurePageResident(pageTable, 0);

    // Grab the process's specific symbol table and declare the variable
    process->getSymbolTable().setVariable(this->varName, PrimitiveType::UINT16, this->defaultValue);

    Symbol var = process->getSymbolTable().getVariable(this->varName);

    // for debugging purposes only
    //std::string finalOutput = "Declared " + varName + " = " + convertPrimitiveToString(var.value);

    // for debugging purposes only
    //FileLogger::logCommandExecution(process->getName(), process->getCPUCoreID(), finalOutput);
}
