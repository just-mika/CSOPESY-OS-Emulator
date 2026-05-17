// cli.h : Declares CLI command handling functions.
//
// Description:
//   Provides interface for processing user commands entered in the shell.
//

#pragma once
#include "system_state.h"
#include <string>

void handleCommand(SystemState& state, std::string input);