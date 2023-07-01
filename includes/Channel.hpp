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

#include "Client.hpp"

typedef std::pair<bool, Client> ChannelUser;

class Channel {
   private:
    std::vector<ChannelUser> channelUsers;
    std::string key;
    
   public:
    Channel(Client &creator, std::string key);
    void addClient(Client client);
    ChannelUser* getChannelUser(Client &client);
    bool removeUser(Client& user);
    void addUser(ChannelUser& user);
    
    std::string getKey();

};

#endif