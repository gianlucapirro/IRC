#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <iostream>
#include <vector>

class Server;

class CommandHandler {
   public:
    CommandHandler(Server* server);
    ~CommandHandler() {}

    void handleCommand(int clientFD, const std::string& command, const std::vector<std::string>& args);
    std::pair<std::string, std::vector<std::string> > parseCommand(const std::string& line);
    std::vector<std::pair<std::string, std::vector<std::string> > > parseCommands(const std::string& lines);

   private:
    Server* server;

    // commands
    void handlePass(int clientFD, const std::vector<std::string>& args);
    void handleNick(int clientFD, const std::vector<std::string>& args);
};

#endif