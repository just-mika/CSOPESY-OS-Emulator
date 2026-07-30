#pragma once
#include "ICommand.h"
#include <string>
#include <cstdint>

class ReadCommand : public ICommand
{
public:
    ReadCommand(int pid, std::string targetVar, uint32_t memoryAddress);
    void execute() override;

private:
    std::string targetVar;
    uint32_t memoryAddress;
};

