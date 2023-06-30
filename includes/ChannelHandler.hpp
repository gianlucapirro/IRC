#ifndef SERVER_HPP
#define SERVER_HPP

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include "Channel.hpp"

class ChannelHandler {
   private:
    std::vector<Channel> channels;

   public:
    ChannelHandler();
};

#endif
