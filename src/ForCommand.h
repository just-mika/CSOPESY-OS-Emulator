#pragma once
#include "ICommand.h"
#include <vector>
#include <memory>

class ForCommand : public ICommand
{
public:
    ForCommand(int pid, std::vector<std::shared_ptr<ICommand>> loopInstructions, int repeats);
    void execute() override;

	int getNestingDepth() const; // Returns the nesting depth of the ForCommand
    void setBodyRange(int start, int end);
    friend class Process;
private:
    std::vector<std::shared_ptr<ICommand>> loopInstructions;
    int repeats;
    int depth; // Stores the depth of this loop
    int bodyStart;
    int bodyEnd;
};