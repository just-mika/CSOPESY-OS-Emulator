#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <filesystem> 

//FOR DEBUGGING ONLY!!! this shouldn't be used in the final submission.W
class FileLogger {
private:
    // Centrally managed directory so everything matches perfectly
    inline static const std::string DIRECTORY_PATH = "output/";

    // Internal helper to handle directory safety quietly
    static void ensureDirectoryExists() {
        try {
            if (!std::filesystem::exists(DIRECTORY_PATH)) {
                std::filesystem::create_directories(DIRECTORY_PATH);
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "[Logger Error] Directory creation failed: " << e.what() << std::endl;
        }
    }

public:
    // Called once inside Process::initializeCommands()
    static void initializeProcessFile(const std::string& processName) {
        ensureDirectoryExists(); // Set up your folder layout immediately

        std::string fullFilePath = DIRECTORY_PATH + processName + ".txt";
        std::ofstream outFile(fullFilePath, std::ios::out); // Overwrites previous session debug records safely

        if (outFile.is_open()) {
            outFile << "Process name: " << processName << "\n";
            outFile << "Logs:\n\n";
            outFile.close();
        }
        else {
            std::cerr << "[Logger Error] Unable to initialize file at: " << fullFilePath << std::endl;
        }
    }

    // Called during execution loops inside your individual ICommands
    static void logCommandExecution(const std::string& processName, int coreID, const std::string& executedText) {
        ensureDirectoryExists();

        // 1. Generate the standardized assignment timestamp layout [cite: 60, 64]
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm localTime;
#ifdef _WIN32
        localtime_s(&localTime, &t);
#else
        localtime_r(&t, &localTime);
#endif
        std::ostringstream timeStream;
        timeStream << std::put_time(&localTime, "(%m/%d/%Y %I:%M:%S%p)"); // Matches spec requirements [cite: 60, 64]
        std::string timestamp = timeStream.str();

        // 2. Combine the path dynamically
        std::string fullFilePath = DIRECTORY_PATH + processName + ".txt";
        std::ofstream outFile(fullFilePath, std::ios::app); // Appends cleanly

        if (outFile.is_open()) {
            // Formats exactly like the specification mockup: (MM/DD/YYYY HH:MM:SSXM) Core:X "Message" [cite: 60, 61, 62, 87, 88]
            outFile << timestamp << " Core:" << coreID << " " << executedText << "\n";
            outFile.close();
        }
        else {
            std::cerr << "[Logger Error] Unable to append file at: " << fullFilePath << std::endl;
        }
    }
};