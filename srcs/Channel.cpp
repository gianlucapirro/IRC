#include <iostream>

#include "Channel.hpp"

Channel::Channel(Client *creator, std::string key) : key(key) {
    this->addUser(creator, true);
}

ChannelUser* Channel::getChannelUser(Client *client) {

    for (size_t i = 0; i < this->channelUsers.size(); i++) {
        if (this->channelUsers[i]->client->getFD() == client->getFD()) {
            return this->channelUsers[i];
        }
    }
    return NULL;
}

bool Channel::removeUser(Client* user) {

    for (size_t i = 0; i < this->channelUsers.size(); i++) {
        if (this->channelUsers[i]->client->getFD() == user->getFD()) {
            delete this->channelUsers[i];
            this->channelUsers.erase(this->channelUsers.begin() + i);
            return true;
        }
    }
    return false;
}


std::string Channel::getKey() {
    return this->key;
}

void Channel::addUser(Client *client, bool isOperator) {
    ChannelUser *channelUser = new ChannelUser;
    channelUser->isOperator = isOperator;
    channelUser->client = client;
    this->channelUsers.push_back(channelUser);
}

void Channel::sendMsg(Client* client, const std::vector<std::string>& args, std::queue<message> *messageQueue) {
    std::string response = client->getNick() + " " + "PRIVMSG" + " ";

    std::ostringstream oss;
    for(size_t i = 1; i < args.size(); ++i) {
        if(i != 0)
            oss << " ";
        oss << args[i];
    }
    response += oss.str();

    for (size_t i = 0; i < this->channelUsers.size(); i++) {
        ChannelUser *user = this->channelUsers[i];
        messageQueue->push(std::make_pair(user->client->getFD(), response));
    }
}
