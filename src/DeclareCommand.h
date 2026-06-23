#pragma once
#include "ICommand.h"
#include <string>
#include <cstdint>

class DeclareCommand : public ICommand
{
public:
    DeclareCommand(int pid, std::string processName, std::string varName, uint16_t defaultValue);
    void execute() override;

private:
    std::string processName;
    std::string varName;
    uint16_t defaultValue;
};
