#pragma once
#include "ICommand.h"
#include <string>
#include <cstdint>

class SleepCommand : public ICommand
{
public:
    SleepCommand(int pid, std::string processName, uint8_t ticksToSleep);
    void execute() override;

private:
    std::string processName;
    uint8_t ticksToSleep;
};
