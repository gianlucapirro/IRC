#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <iostream>
#include <vector>

typedef std::pair<std::string, std::vector<std::string> > parsedCommand;

class Server;

class CommandHandler {
   public:
    CommandHandler(Server* server);
    ~CommandHandler() {
    }

    void handleCommand(int clientFD, const std::string& command, const std::vector<std::string>& args);
    parsedCommand parseCommand(const std::string& line);
    std::vector<parsedCommand> parseCommands(const std::string& lines);

   private:
    Server* server;

    // commands
    void handleCap(int clientFD, const std::vector<std::string>& args);
    void handlePass(int clientFD, const std::vector<std::string>& args);
    void handleNick(int clientFD, const std::vector<std::string>& args);
    void handleUser(int clientFD, const std::vector<std::string>& args);
};

#endif