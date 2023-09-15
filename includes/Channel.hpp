#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <vector>
#include <queue>

#include "Client.hpp"
#include "ResponseBuilder.hpp"

typedef std::pair<int, std::string> message; //TODO: make this client instead of using the fd

typedef struct {
    bool isOperator;
    Client *client;
} ChannelUser;

class Channel {
   private:
    std::vector<ChannelUser *> channelUsers;
    std::string key;
    
   public:
    Channel(Client* creator, std::string key);
    ChannelUser* getChannelUser(Client* client);
    bool removeUser(Client* user);
    void addUser(Client *client, bool isOperator);
    void sendMsg(Client* client, const std::vector<std::string>& args, std::queue<message> *messageQueue);
    void broadcast(std::string msg, std::queue<message> *messageQueue);
    
    std::string getKey();

};

#endif