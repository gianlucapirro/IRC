
#include "ChannelHandler.hpp"

ChannelHandler::ChannelHandler() {

}

void ChannelHandler::join(Client* client, const std::vector<std::string>& args, std::queue<message> *messageQueue) {
    // TODO: check the name on wrong characters
    Channel *currentChannel = this->getChannelByClient(client);
    if (currentChannel) {
        currentChannel->removeUser(client);
    }

    Channel *channel = this->getChannelByKey(args[0]);
    if (channel == NULL) {
        Channel *newChannel = new Channel(client, args[0]);
        this->channels.push_back(newChannel);
    } else {
        channel->addUser(client, false);
    }

    std::string topicResponse = ResponseBuilder("ircserv"
        ).addCommand(RPL_NOTOPIC
        ).addTrailing("No topic is set"
        ).build();

    messageQueue->push(std::make_pair(client->getFD(), topicResponse));

    std::string namesResponse = ResponseBuilder("ircserv"
        ).addCommand(RPL_NAMREPLY 
        ).addTrailing("names"
        ).build();
    // TODO: put real users here

    messageQueue->push(std::make_pair(client->getFD(), namesResponse));

}

void ChannelHandler::handleMsg(Client* client, const std::vector<std::string>& args, std::queue<message> *messageQueue) {
    Channel *channel = this->getChannelByKey(args[0]);
    if (channel != NULL) {
        channel->sendMsg(client, args, messageQueue);
    }
}

Channel *ChannelHandler::getChannelByClient(Client *client) {

    for (size_t i = 0; i < this->channels.size(); i++) {
        Channel* channel = this->channels[i];
        if (channel->getChannelUser(client) != NULL) {
            return channel;
        }
    }
    return NULL;
}

Channel *ChannelHandler::getChannelByKey(std::string key) {

    for (size_t i = 0; i < this->channels.size(); i++) {
        Channel *channel = this->channels[i];
        if (channel->getKey() == key) {
            return channel;
        }
    }
    return NULL;
}