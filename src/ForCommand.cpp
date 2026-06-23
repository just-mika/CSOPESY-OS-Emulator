#include "ForCommand.h"
#include <stdexcept> // For throwing errors
#include <iostream>

ForCommand::ForCommand(int pid, std::vector<std::shared_ptr<ICommand>> loopInstructions, int repeats)
    : ICommand(pid, CommandType::FOR), loopInstructions(loopInstructions), repeats(repeats)
{
    int maxInnerDepth = 0;
    for (const auto& command : this->loopInstructions) {

        // If we find another FOR loop inside
        if (command->getCommandType() == CommandType::FOR) {

            // Safely cast it to a ForCommand so we can read its depth
            auto innerFor = std::dynamic_pointer_cast<ForCommand>(command);
            if (innerFor) {
                if (innerFor->getNestingDepth() > maxInnerDepth) {
                    maxInnerDepth = innerFor->getNestingDepth();
                }
            }
        }
    }

	// This loop + inner loops
    this->depth = maxInnerDepth + 1;

    if (this->depth > 3) {
		std::cerr << "ERROR: For loop nesting exceeds the maximum limit of 3!" << std::endl; // Print an error message if the nesting depth exceeds 3
    }
}

void ForCommand::execute()
{
	// Execute the loop instructions for the specified number of repeats
    for (int i = 0; i < repeats; ++i) {
        for (const auto& command : loopInstructions) {
            command->execute();
        }
    }
}

int ForCommand::getNestingDepth() const
{
    return this->depth;
}
