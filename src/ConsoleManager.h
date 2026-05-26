#pragma once
#include "System.h"
#include <string>
#include "AConsole.h"
#include <unordered_map>
#include "BaseScreen.h"

class ConsoleManager
{
public:
    typedef std::unordered_map<std::string, std::shared_ptr<AConsole>> ConsoleTable;

    static ConsoleManager* getInstance();
    static void initialize();
    static void destroy();

    void drawConsole() const;
    void process() const;
    void switchConsole(std::string consoleName);

    void registerScreen(std::shared_ptr<BaseScreen> screenRef);
    void switchToScreen(std::string screenName);
    void unregisterScreen(std::string screenName);

    void returnToPreviousConsole();
    void exitApplication();
    bool isRunning() const;

private:
    ConsoleManager();
    ~ConsoleManager() = default;
    ConsoleManager(ConsoleManager const&) {} //copy constructor is private
    ConsoleManager& operator = (ConsoleManager const&) {} //assignment operator is private
    static ConsoleManager* sharedInstance;

    ConsoleTable consoleTable;
    std::shared_ptr<AConsole> currentConsole;
    std::shared_ptr<AConsole> previousConsole;

    bool running = true;
};