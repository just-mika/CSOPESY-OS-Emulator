#include "System.h"
#include "Console.h"

#include <iostream>

int main()
{
    System system;
    Console console(system);

    console.run();
    
    return 0;
}