#ifndef CHANNEL_HANDLER_HPP
#define CHANNEL_HANDLER_HPP

#include <iostream>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>

#include <iostream>
#include <vector>
#include <queue>

#include "Channel.hpp"
#include "ResponseBuilder.hpp"
#include "utils.hpp"
#include "commands.hpp"

typedef std::pair<int, std::string> message;

struct ModeChange {
    int operation;
    char mode;
    Channel* channel;
    std::string argument;
};

class ChannelHandler {
   private:
    std::vector<Channel*> channels;
    std::queue<message> *messageQueue;
    std::vector<Client*> *clients;  // vector of connected clients

    ChannelHandler();
    Channel* getChannelByClient(Client* client);
    Channel* getChannelByKey(std::string key);

    int modeGetOperatorOrRespond(Client* client, std::string modeString);
    Channel* modeGetChannelOrRespond(Client* client, const std::string& name);
    std::vector<ModeChange> modeParseArgsOrRespond(Client *client, const std::vector<std::string>& args);

    bool modeSetOperator(Client* client, ModeChange change);
    bool modeSetLimit(Client* client, ModeChange change);
    bool modeSetPass(Client* client, ModeChange change);
    bool modeSetTopic(Client* client, ModeChange change);
    bool modeSetInviteOnly(Client* client, ModeChange change);

   public:
    ChannelHandler(std::queue<message>* messageQueue, std::vector<Client*> *clients);
    void join(Client *client, const std::vector<std::string>& args, std::queue<message> *messageQueue);
    void handleMsg(Client* client, const std::vector<std::string>& args, std::queue<message> *messageQueue);
    void handleKick(Client* client, const std::vector<std::string>& channelsToKick, const std::vector<std::string>& clientsToKick, std::vector<Client*> *clients, std::queue<message> *messageQueue, std::string reason);
    void handleLeave(Client* client, const std::vector<std::string>& channelsToLeave, std::queue<message> *messageQueue);
    void handleMode(Client* client, const std::vector<std::string>& args);
    void handleTopic(Client* client, const std::vector<std::string>& args);
    void handleInvite(Client* client, const std::vector<std::string>& args);
    void removeClientFromAllChannels(Client* client);

};

#endif
