#include "MathCommand.h"
#include "GlobalScheduler.h"
#include "Process.h"
#include <algorithm>

#include "FileLogger.h"

MathCommand::MathCommand(int pid, std::string targetVar, Operand op1, Operand op2, CommandType operation)
    : ICommand(pid, operation), targetVar(targetVar), op1(op1), op2(op2)
{
}

uint16_t MathCommand::evaluateOperand(const Operand& op, std::shared_ptr<Process> process)
{
    if (std::holds_alternative<uint16_t>(op)) {
        return std::get<uint16_t>(op);
    }
    else {
        std::string varName = std::get<std::string>(op);
        PrimitiveValue val = process->getSymbolTable().getVariable(varName).value;

        if (!std::holds_alternative<uint16_t>(val)) {
            process->getSymbolTable().setVariable(varName, PrimitiveType::UINT16, static_cast<uint16_t>(0));
            val = process->getSymbolTable().getVariable(varName).value;
        }

        return std::get<uint16_t>(val);
    }
}

void MathCommand::execute()
{
    std::shared_ptr<Process> process = GlobalScheduler::getInstance()->findProcess(pid);

    if (process) {
        uint16_t val1 = evaluateOperand(this->op1, process);
        uint16_t val2 = evaluateOperand(this->op2, process);

        std::string finalOutput;

        int result = 0;

        if (this->commandType == CommandType::ADD) {
            finalOutput = "ADD ";
			result = static_cast<int>(val1) + static_cast<int>(val2); // Use int to prevent overflow during addition
        }
        else if (this->commandType == CommandType::SUBTRACT) {
            finalOutput = "SUBTRACT ";
            result = static_cast<int>(val1) - static_cast<int>(val2);
        }

		result = std::clamp(result, 0, 65535); // Clamp the result to the range of uint16_t

		process->getSymbolTable().setVariable(this->targetVar, PrimitiveType::UINT16, static_cast<uint16_t>(result)); // Store the final result as uint16_t
    
		
        Symbol var = process->getSymbolTable().getVariable(this->targetVar);

        // for debugging purposes only
        //finalOutput += std::to_string(val1) + " + " + std::to_string(val2) + " = " + convertPrimitiveToString(var.value);

        // for debugging purposes only
        //FileLogger::logCommandExecution(process->getName(), process->getCPUCoreID(), finalOutput);

    }
}