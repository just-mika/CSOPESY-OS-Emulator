#pragma 
#include "AConsole.h"

#include "System.h"

class MainConsole : public AConsole
{
public:
    MainConsole();

    void display() override;
    void process() override;
    void onEnabled() override;

private:
    void handleCommand(const std::string& input);
    friend class System;
};