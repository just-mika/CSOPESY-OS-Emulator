#pragma once
#include "ICommand.h"
#include <string>
#include <cstdint>

class SleepCommand : public ICommand
{
public:
    SleepCommand(int pid, uint8_t ticksToSleep);
    void execute() override;

private:
    uint8_t ticksToSleep;
};
