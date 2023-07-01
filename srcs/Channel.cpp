#include <iostream>

#include "Channel.hpp"

Channel::Channel(Client &creator, std::string key) : key(key) {
    ChannelUser channelUser = std::make_pair(true, creator);
    this->addUser(channelUser);
}

ChannelUser* Channel::getChannelUser(Client &client) {

    for (size_t i = 0; i < this->channelUsers.size(); i++) {
        if (this->channelUsers[i].second.getFD() == client.getFD()) {
            return &(this->channelUsers[i]);
        }
    }
    return NULL;
}

bool Channel::removeUser(Client& user) {

    for (size_t i = 0; i < this->channelUsers.size(); i++) {
        if (this->channelUsers[i].second.getFD() == user.getFD()) {
            this->channelUsers.erase(this->channelUsers.begin() + i);
            return true;
        }
    }
    return false;
}


std::string Channel::getKey() {
    return this->key;
}

void Channel::addUser(ChannelUser& channelUser) {
    this->channelUsers.push_back(channelUser);
}