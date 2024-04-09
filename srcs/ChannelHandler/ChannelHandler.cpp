#include "ChannelHandler.hpp"

ChannelHandler::ChannelHandler(std::vector<Client*>* clients) {
    this->clients = clients;
}

void ChannelHandler::handleMsg(Client* client, const std::vector<std::string>& args) {
    Channel* channel = this->getChannelByKey(args[0]);
    if (channel == NULL) {
        respond(client->getFD(), AERR_NOSUCHCHANNEL(client->getNick(), args[0]));
        return;
    }

    ChannelUser* channelUser = channel->getChannelUser(client);
    if (channelUser == NULL) {
        respond(client->getFD(), AERR_NOTONCHANNEL(client->getNick(), channel->getKey()));
        return;
    }
    channel->sendMsg(client, args);
}

void ChannelHandler::handleKick(Client* client, const std::vector<std::string>& channelsToKick,
                                const std::vector<std::string>& clientsToKick, std::string reason) {
    std::vector<Channel*> foundChannels;
    for (size_t i = 0; i < channelsToKick.size(); i++) {
        Channel* foundChannel = this->getChannelByKey(channelsToKick[i]);
        if (foundChannel == NULL) {
            respond(client->getFD(), AERR_NOSUCHCHANNEL(client->getNick(), channelsToKick[i]));
            return;
        }
        foundChannels.push_back(foundChannel);
    }

    std::vector<Client*> foundClients;
    for (size_t i = 0; i < clientsToKick.size(); i++) {
        Client* foundClient = getClientByNick(clients, clientsToKick[i]);
        if (foundClient != NULL) {
            foundClients.push_back(foundClient);
        }
    }

    std::vector<Channel*> possiblyDelete;

    for (size_t i = 0; i < foundChannels.size(); i++) {
        for (size_t p = 0; p < foundClients.size(); p++) {
            foundChannels[i]->removeUser(foundClients[p]);
            possiblyDelete.push_back(foundChannels[i]);
            std::string response =
                ARPL_KICK(client->getFullClientIdentifier(), foundChannels[i]->getKey(), foundClients[p]->getNick(), reason);
            foundChannels[i]->broadcast(response);
            respond(foundClients[p]->getFD(), response);
        }
    }

    for (size_t i = 0; i < possiblyDelete.size(); i++) {
        this->removeChannelIfEmpty(possiblyDelete[i]);
    }
}

Channel* ChannelHandler::getChannelByKey(std::string key) {
    for (size_t i = 0; i < this->channels.size(); i++) {
        Channel* channel = this->channels[i];
        if (channel->getKey() == key) {
            return channel;
        }
    }
    return NULL;
}

void ChannelHandler::handleTopic(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 1) {
        respond(client->getFD(), AERR_NEEDMOREPARAMS(client->getNick(), "TOPIC"));
        return;
    }

    Channel* foundChannel = this->getChannelByKey(args[0]);
    if (foundChannel == NULL) {
        respond(client->getFD(), AERR_NOSUCHCHANNEL(client->getNick(), args[0]));
        return;
    }

    ChannelUser* channelUser = foundChannel->getChannelUser(client);
    if (channelUser == NULL) {
        respond(client->getFD(), AERR_NOTONCHANNEL(client->getNick(), foundChannel->getKey()));
        return;
    }

    if (foundChannel->getCanChangeTopic() == false && channelUser->isOperator == false) {
        respond(client->getFD(), AERR_CHANOPRIVSNEEDED(client->getNick(), foundChannel->getKey()));
        return;
    }

    if (args.size() == 1) {
        if (foundChannel->getTopic() == "") {
            respond(client->getFD(), ARPL_NOTOPIC(client->getNick(), foundChannel->getKey()));

        } else {
            respond(client->getFD(), ARPL_TOPIC(client->getFullClientIdentifier(), foundChannel->getKey(), foundChannel->getTopic()));
        }
        return;
    }

    if (args[1].length() < 2 || args[1][0] != ':') {
        respond(client->getFD(), AERR_NEEDMOREPARAMS(client->getNick(), "TOPIC"));
        return;
    }

    std::string newTopic = args[1].substr(1, args[1].size() - 1);
    foundChannel->setTopic(newTopic);
    std::string response = ARPL_TOPIC(client->getFullClientIdentifier(), foundChannel->getKey(), foundChannel->getTopic());
    foundChannel->broadcast(response);
    return;
}

void ChannelHandler::removeClientFromAllChannels(Client* client) {
    std::vector<Channel*> possiblyDelete;
    for (size_t i = 0; i < this->channels.size(); i++) {
        Channel* channel = this->channels[i];
        if (channel->getChannelUser(client) == NULL) {
            continue;
        }
        std::string formattedClient = client->getNick();
        std::string response = ARPL_PART(client->getFullClientIdentifier(), channel->getKey(), "Leaving");
        channel->broadcast(response);
        channel->removeUser(client);
        possiblyDelete.push_back(channel);
    }

    for (size_t i = 0; i < possiblyDelete.size(); i++) {
        this->removeChannelIfEmpty(possiblyDelete[i]);
    }
}

void ChannelHandler::removeChannelIfEmpty(Channel* channel) {
    if (channel->getUserCount() > 0) return;
    for (size_t i = 0; i < this->channels.size(); i++) {
        if (this->channels[i] == channel) {
            this->channels.erase(this->channels.begin() + i);
            delete channel;
            return;
        }
    }
}