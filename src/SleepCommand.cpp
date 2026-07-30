#include "SleepCommand.h"

#include "FileLogger.h"
#include "GlobalScheduler.h"
#include "Process.h"

SleepCommand::SleepCommand(int pid, uint8_t ticksToSleep)
    : ICommand(pid, CommandType::SLEEP), ticksToSleep(ticksToSleep)
{
}

void SleepCommand::execute()
{
    std::shared_ptr<Process> process = GlobalScheduler::getInstance()->findProcess(pid);

    if (process) {
		process->setRemainingSleepTicks(this->ticksToSleep);    // Set the remaining sleep ticks for the process
		process->sleepProcess();                                // Change the process state to WAITING
    }

    // For debugging purposes only
    //FileLogger::logCommandExecution(process->getName(), process->getCPUCoreID(), "SLEEP for " + std::to_string(ticksToSleep));
}