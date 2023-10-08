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

#include "response.hpp"
#include "Channel.hpp"
#include "utils.hpp"

struct ModeChange {
    int operation;
    char mode;
    Channel* channel;
    std::string argument;
};

class ChannelHandler {
   private:
    std::vector<Channel*> channels;
    std::vector<Client*> *clients;  // vector of connected clients

    ChannelHandler();
    Channel* getChannelByKey(std::string key);

    int modeGetOperatorOrRespond(Client* client, std::string modeString);
    Channel* modeGetChannelOrRespond(Client* client, const std::string& name);
    Channel* getOrCreateChannel(Client* client, const std::vector<std::string>& args);
    std::vector<ModeChange> modeParseArgsOrRespond(Client *client, const std::vector<std::string>& args);

    void removeChannelIfEmpty(Channel* channel);

    bool modeSetOperator(Client* client, ModeChange change);
    bool modeSetLimit(Client* client, ModeChange change);
    bool modeSetPass(Client* client, ModeChange change);
    bool modeSetTopic(Client* client, ModeChange change);
    bool modeSetInviteOnly(Client* client, ModeChange change);

   public:
    ChannelHandler(std::vector<Client*> *clients);
    void join(Client *client, const std::vector<std::string>& args);
    void handleMsg(Client* client, const std::vector<std::string>& args);
    void handleKick(Client* client, const std::vector<std::string>& channelsToKick, const std::vector<std::string>& clientsToKick, std::string reason);
    void handleLeave(Client* client, const std::vector<std::string>& args);
    void handleMode(Client* client, const std::vector<std::string>& args);
    void handleTopic(Client* client, const std::vector<std::string>& args);
    void handleInvite(Client* client, const std::vector<std::string>& args);
    void removeClientFromAllChannels(Client* client);

};

#endif
