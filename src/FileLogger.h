#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <filesystem> // Requires C++17 or newer

class FileLogger {
private:
    // Define your target directory here (can be relative or absolute like "C:/csopesy-logs/")
    inline static const std::string DIRECTORY_PATH = "output/";

public:
    static void logCommandExecution(const std::string& processName, int coreID, const std::string& executedText) {
        // 1. Ensure the target directory actually exists
        try {
            if (!std::filesystem::exists(DIRECTORY_PATH)) {
                std::filesystem::create_directories(DIRECTORY_PATH); // Creates the folder structure if missing
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "[Logger Error] Directory creation failed: " << e.what() << std::endl;
            return;
        }

        // 2. Generate the standardized timestamp
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

        // 3. Combine the directory path with the process filename
        std::string fullFilePath = DIRECTORY_PATH + processName + ".txt";
        std::ofstream outFile(fullFilePath, std::ios::app);

        if (outFile.is_open()) {
            // Formats exactly like the specification mockup [cite: 60, 61, 62]
            outFile << timestamp << " Core:" << coreID << " " << executedText << "\n";
            outFile.close();
        }
        else {
            std::cerr << "[Logger Error] Unable to open/create file at: " << fullFilePath << std::endl;
        }
    }
};