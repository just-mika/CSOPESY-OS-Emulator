#pragma once
#include "ICommand.h"
#include <string>
#include <variant>
#include <cstdint>
#include <memory>

#include "Process.h"

using Operand = std::variant<std::string, uint16_t>;

class MathCommand : public ICommand
{
public:
    MathCommand(int pid, std::string targetVar, Operand op1, Operand op2, CommandType operation);
    void execute() override;

private:
    std::string targetVar;
    Operand op1;
    Operand op2;

    uint16_t evaluateOperand(const Operand& op, std::shared_ptr<Process> process);
};

