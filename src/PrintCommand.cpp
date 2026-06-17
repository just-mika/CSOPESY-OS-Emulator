#include "PrintCommand.h"
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iostream>

PrintCommand::PrintCommand(int pid, std::string processName, std::string& toPrint)
    : ICommand(pid, CommandType::PRINT), processName(processName), toPrint(toPrint)
{
}

void PrintCommand::execute()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;

#ifdef _WIN32
    localtime_s(&localTime, &t);
#else
    localtime_r(&t, &localTime);
#endif
    std::ostringstream timeStream;
    timeStream << std::put_time(&localTime, "(%m/%d/%Y %I:%M:%S%p)");
    std::string timestamp = timeStream.str();

    std::string fileName = this->processName + ".txt"; // Create file name based on process name
    std::ofstream outFile(fileName, std::ios::app);    // Open file in append mode to add new content without overwriting

    if (outFile.is_open()) {
        outFile << timestamp << " Core:" << this->activeCoreID << " \"" << this->toPrint << "\"\n";  // Save to text file with timestamp and core ID
        outFile.close();
    }
    else {
        std::cerr << "Unable to open file: " << fileName << std::endl; // Handle error if file cannot be opened
    }
}

void PrintCommand::setCoreID(int coreID)
{
    this->activeCoreID = coreID;
}
