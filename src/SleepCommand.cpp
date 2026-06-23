#include "SleepCommand.h"
#include "GlobalScheduler.h"
#include "Process.h"

SleepCommand::SleepCommand(int pid, std::string processName, uint8_t ticksToSleep)
    : ICommand(pid, CommandType::SLEEP), processName(processName), ticksToSleep(ticksToSleep)
{
}

void SleepCommand::execute()
{
    std::shared_ptr<Process> process = GlobalScheduler::getInstance()->findProcess(this->processName);

    if (process) {
		process->setRemainingSleepTicks(this->ticksToSleep);    // Set the remaining sleep ticks for the process
		process->sleepProcess();                                // Change the process state to WAITING
    }
}