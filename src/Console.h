#pragma once
#include "System.h"

class Console {
private:
    System& system;

public:
    Console(System& system);

    void run();
    void handleCommand(const std::string& input);
};