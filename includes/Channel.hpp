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

class Channel {
   private:
    std::vector<std::pair<bool, Client> > channelUsers;
    
   public:
    Channel(Client creator);
    void addClient(Client client);

};

#endif