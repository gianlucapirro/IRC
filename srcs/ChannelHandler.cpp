
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

void ChannelHandler::handleKick(Client* client, const std::vector<std::string>& channelsToKick, const std::vector<std::string>& clientsToKick, std::vector<Client*> *clients, std::queue<message> *messageQueue) {
    std::vector<Channel*> foundChannels;
    for (int i = 0; i < channelsToKick.size(); i++) {
        Channel* foundChannel = this->getChannelByKey(channelsToKick[i]);
        if (foundChannel == NULL) {
            std::string response = ResponseBuilder("ircserv").addCommand("403").addParameters(channelsToKick[i]).addTrailing("No such channel").build();
            messageQueue->push(std::make_pair(client->getFD(), response));
            return;
        }
        foundChannels.push_back(foundChannel);
    }

    for (int i = 0; i < foundChannels.size(); i++) {
        ChannelUser* channelUser = foundChannels[i]->getChannelUser(client);
        if (channelUser == NULL) {
            std::string response = ResponseBuilder("ircserv").addCommand("442").addParameters(channelsToKick[i]).addTrailing("You're not on that channel").build();
            messageQueue->push(std::make_pair(client->getFD(), response));
            return;
        }
        if (!channelUser->isOperator) {
            std::string response = ResponseBuilder("ircserv").addCommand("482").addParameters(channelsToKick[i]).addTrailing("You're not channel operator").build();
            messageQueue->push(std::make_pair(client->getFD(), response));
            return;
        }
    }

    std::vector<Client*> foundClients;
    for (int i = 0; i < clientsToKick.size(); i++) {
        Client* foundClient = getClientByUsername(clients, clientsToKick[i]);
        if (foundClient != NULL) {
            foundClients.push_back(foundClient);
        }
    }

    for (int i = 0; i < foundChannels.size(); i++) {
        for (int p = 0; p < foundClients.size(); p++) {
            foundChannels[i]->removeUser(foundClients[p]);
        }
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

