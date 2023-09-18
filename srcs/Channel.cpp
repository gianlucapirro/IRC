#include <iostream>

#include "Channel.hpp"

Channel::Channel(Client *creator, std::string key) : key(key), limit(0), pass(""), topic(""), canChangeTopic(false) {
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
    std::string response = ":" + client->getNick() + " PRIVMSG";

    std::ostringstream oss;
    for(size_t i = 0; i < args.size(); ++i) {
        oss << " ";
        oss << args[i];
    }
    response += oss.str();
    response += "\r\n";

    for (size_t i = 0; i < this->channelUsers.size(); i++) {
        ChannelUser *user = this->channelUsers[i];
        if (user->client->getFD() != client->getFD()) {
            messageQueue->push(std::make_pair(user->client->getFD(), response));
        }
    }
}

void Channel::broadcast(std::string msg, std::queue<message> *messageQueue) {
    msg += "\r\n";

    for (size_t i = 0; i < this->channelUsers.size(); i++) {
        ChannelUser *user = this->channelUsers[i];
        messageQueue->push(std::make_pair(user->client->getFD(), msg));
    }
}

std::string Channel::getNicknames() {
    std::string names = "";
    if (this->channelUsers.size() == 0)
        return names;
    for (size_t i = 0; i < this->channelUsers.size(); i++) {
        ChannelUser* channelUser = this->channelUsers[i];
        names += channelUser->client->getNick();
        if (i != this->channelUsers.size() - 1)
            names += " ";
    }
    return names;
}

int Channel::getLimit() {
    return this->limit;
}

int Channel::setLimit(int limit) {
    this->limit = limit;
    return this->limit;
}

int Channel::getUserCount() {
    return this->channelUsers.size();
}

std::string Channel::getPass() {
    return this->pass;
}

std::string Channel::setPass(std::string pass) {
    this->pass = pass;
    return this->pass;
}

std::string Channel::getTopic() {
    return this->topic;
}

std::string Channel::setTopic(std::string topic) {
    this->topic = topic;
    return this->topic;
}

bool Channel::getCanChangeTopic() {
    return this->canChangeTopic;
}

bool Channel::setCanChangeTopic(bool canChangeTopic) {
    this->canChangeTopic = canChangeTopic;
    return this->canChangeTopic;
}