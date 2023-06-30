#include <iostream>

#include "Channel.hpp"

Channel::Channel(Client creator) {
    this->channelUsers.push_back(std::make_pair(true, creator));
}
