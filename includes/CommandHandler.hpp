#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <queue>

#include "ChannelHandler.hpp"
#include "Config.hpp"
#include "Client.hpp"
#include "utils.hpp"

#define NO_ACTION 1
#define QUIT_USER 2

typedef std::pair<std::string, std::vector<std::string> > parsedCommand;

class CommandHandler {
   public:
    ChannelHandler channelHandler;

    CommandHandler(const Config *config, std::vector<Client*> *clients);
    // ~CommandHandler();

    void handleCommand(Client* client, const std::string& command, const std::vector<std::string>& args);
    bool isNicknameInUse(const std::string& nickname) const;

    parsedCommand parseCommand(const std::string& line);
    std::vector<parsedCommand> parseCommands(const std::vector<std::string> commands);


   private:
    const Config *config;
    std::vector<Client*> *clients;  // vector of connected clients

    void handleRegistration(Client *client, const std::string& command, const std::vector<std::string>& args);

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
    void handleTopic(Client* client, const std::vector<std::string>& args);
    void handleInvite(Client* client, const std::vector<std::string>& args);
    void handlePing(Client* client) const;


    Client* searchClient(int clientFD);
};

#endif