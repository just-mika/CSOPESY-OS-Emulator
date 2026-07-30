#pragma once
#include "ICommand.h"
#include <string>
#include <cstdint>

class WriteCommand : public ICommand
{
public:
    // Write literal value to address
    WriteCommand(int pid, uint32_t memoryAddress, uint16_t literalValue);

    // Write value of a variable to address
    WriteCommand(int pid, uint32_t memoryAddress, std::string sourceVar);

    void execute() override;

private:
    uint32_t memoryAddress;
    uint16_t literalValue = 0;
    std::string sourceVar = "";
    bool isFromVariable = false;
};