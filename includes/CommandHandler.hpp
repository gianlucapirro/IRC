#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <queue>

#include "ChannelHandler.hpp"
#include "ResponseBuilder.hpp"
#include "Config.hpp"
#include "Client.hpp"

#define NO_ACTION 1
#define QUIT_USER 2

typedef std::pair<std::string, std::vector<std::string> > parsedCommand;
typedef std::pair<int, std::string> message; //TODO: make this client instead of using the fd

class CommandHandler {
   public:
    ChannelHandler channelHandler;

    CommandHandler(std::queue<message> *messages, const Config *config, std::vector<Client*> *clients);
    ~CommandHandler() {
    } // TODO: we cant have functions in header file

    int handleCommand(Client* client, const std::string& command, const std::vector<std::string>& args);
    parsedCommand parseCommand(const std::string& line);
    std::vector<parsedCommand> parseCommands(const std::vector<std::string> commands);
    // void CommandHandler::handleBuffer(char* buffer, int valread, int fd);

   private:
    const Config *config;
    std::queue<message> *messageQueue;
    std::vector<Client*> *clients;  // vector of connected clients

    // commands
    void handleCap(Client* client, const std::vector<std::string>& args);
    void handlePass(Client* client, const std::vector<std::string>& args);
    void handleNick(Client* client, const std::vector<std::string>& args);
    void handleUser(Client* client, const std::vector<std::string>& args);
    void handlePrivMsg(Client *client, const std::vector<std::string>& args);
    void handleKick(Client *client, const std::vector<std::string>& args);
    void handleLeave(Client* client, const std::vector<std::string>& args);
    void handleJoin(Client *client, const std::vector<std::string>& args);
    void handleMode(Client* client, const std::vector<std::string>& args);

    Client* searchClient(int clientFD);
    bool isNicknameInUse(const std::string& nickname) const;
};

#endif