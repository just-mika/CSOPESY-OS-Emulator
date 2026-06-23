#include "DeclareCommand.h"
#include "GlobalScheduler.h"
#include "Process.h"

DeclareCommand::DeclareCommand(int pid, std::string processName, std::string varName, uint16_t defaultValue)
    : ICommand(pid, CommandType::DECLARE), processName(processName), varName(varName), defaultValue(defaultValue)
{
}

void DeclareCommand::execute()
{
    std::shared_ptr<Process> process = GlobalScheduler::getInstance()->findProcess(this->processName);

    if (process) {
        // Grab the process's specific symbol table and declare the variable
        process->getSymbolTable().setVariable(this->varName, PrimitiveType::UINT16, this->defaultValue);
    }
}
