#ifndef CHANNEL_HANDLER_HPP
#define CHANNEL_HANDLER_HPP

#include <iostream>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <vector>
#include <queue>

#include "Channel.hpp"
#include "ResponseBuilder.hpp"
#include "utils.hpp"

#define RPL_TOPIC "332"
#define RPL_NAMREPLY "353"
#define RPL_NOTOPIC "331"

typedef std::pair<int, std::string> message;

class ChannelHandler {
   private:
    std::vector<Channel*> channels;

    Channel* getChannelByClient(Client* client);
    Channel* getChannelByKey(std::string key);

   public:
    ChannelHandler();
    void join(Client *client, const std::vector<std::string>& args, std::queue<message> *messageQueue);
    void handleMsg(Client* client, const std::vector<std::string>& args, std::queue<message> *messageQueue);
    void handleKick(Client* client, const std::vector<std::string>& channelsToKick, const std::vector<std::string>& clientsToKick, std::vector<Client*> *clients, std::queue<message> *messageQueue, std::string reason);
    void handleLeave(Client* client, const std::vector<std::string>& channelsToLeave, std::queue<message> *messageQueue);
};

#endif
