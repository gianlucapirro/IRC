#include "ChannelHandler.hpp"

ChannelHandler::ChannelHandler(std::vector<Client*> *clients) {
    this->clients = clients;
}

void ChannelHandler::join(Client* client, const std::vector<std::string>& args) {
    if (!client->getIsAuthenticated()) {
        respond(client->getFD(), AERR_NOTREGISTERED(client->getNick()));
        return;
    }
    // TODO: check the name on wrong characters

    Channel *channel = this->getChannelByKey(args[0]);
    if (channel == NULL) {
        Channel *newChannel = new Channel(client, args[0]);
        this->channels.push_back(newChannel);
        channel = newChannel;
    } else if (channel->getLimit() != 0 && channel->getLimit() <= channel->getUserCount()) {
        respond(client->getFD(), AERR_CHANNELISFULL(client->getNick(), channel->getKey()));
        return;
    } else if (channel->getPass() != "" && (args.size() < 2 || args[1] != channel->getPass())) {
        respond(client->getFD(), AERR_BADCHANNELKEY(client->getNick(), channel->getKey()));
        return;
    } else if (channel->getIsInviteOnly() && !channel->isInvited(client->getNick())) {
        respond(client->getFD(), AERR_INVITEONLYCHAN(client->getNick(), channel->getKey()));
        return;
    } else if (channel->getChannelUser(client) != NULL) {
        return;
    } else {
        channel->addUser(client, false);
    }

    channel->broadcast(ARPL_JOIN(client->getNick(), channel->getKey()));

    // TODO: add topic if it exists
    respond(client->getFD(), ARPL_NOTOPIC(client->getNick(), channel->getKey()));

    std::string names = channel->getNicknames();
    respond(client->getFD(), ARPL_NAMREPLY(client->getNick(), channel->getKey(), names));
    respond(client->getFD(), ARPL_ENDOFNAMES(client->getNick(), channel->getKey()));
}

void ChannelHandler::handleMsg(Client* client, const std::vector<std::string>& args) {
    // TODO: avoid sending to channel that you're not in
    Channel *channel = this->getChannelByKey(args[0]);
    if (channel != NULL) {
        channel->sendMsg(client, args);
    }
}

void ChannelHandler::handleKick(
    Client* client,
    const std::vector<std::string>& channelsToKick,
    const std::vector<std::string>& clientsToKick,
    std::string reason
) {
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

    for (size_t i = 0; i < foundChannels.size(); i++) {
        for (size_t p = 0; p < foundClients.size(); p++) {
            foundChannels[i]->removeUser(foundClients[p]);
            std::string response = ARPL_KICK(client->getNick(),foundChannels[i]->getKey(), foundClients[p]->getNick(), reason);
            respond(foundClients[p]->getFD(), response);
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

    if (args.size() == 1) {
        if (foundChannel->getTopic() == "") {
            respond(client->getFD(), ARPL_NOTOPIC(client->getNick(), foundChannel->getKey()));

        } else {
            respond(client->getFD(), ARPL_TOPIC(client->getNick(), foundChannel->getKey(), foundChannel->getTopic()));
        }
        return;
    }
    if (foundChannel->getCanChangeTopic() == false && channelUser->isOperator == false) {
        respond(client->getFD(), AERR_CHANOPRIVSNEEDED(client->getNick(), foundChannel->getKey()));
        return;
    }
    foundChannel->setTopic(args[1]);
    std::string response = ARPL_TOPIC(client->getNick(), foundChannel->getKey(), foundChannel->getTopic());
    foundChannel->broadcast(response);
    return;
}

void ChannelHandler::handleLeave(Client* client, const std::vector<std::string>& channelsToLeave) {

    std::vector<Channel*> channels;

    for (size_t i = 0; i < channelsToLeave.size(); i++) {

        Channel* channel = this->getChannelByKey(channelsToLeave[i]);
        if (channel == NULL) {
            respond(client->getFD(), AERR_NOSUCHCHANNEL(client->getNick(), channelsToLeave[i]));
            return;
        }
        channels.push_back(channel);
    }

    std::vector<std::pair<ChannelUser*, Channel*> > channelUsers;

    for (size_t i = 0; i < channels.size(); i++) {
        Channel* channel = channels[i];
        ChannelUser* channelUser = channel->getChannelUser(client);
        if (channelUser == NULL) {
            respond(client->getFD(), AERR_NOTONCHANNEL(client->getNick(), channelsToLeave[i]));
            return;
        }
        channelUsers.push_back(std::make_pair(channelUser, channel));
    }

    for (size_t i = 0; i < channelUsers.size(); i++) {
        ChannelUser* channelUser = channelUsers[i].first;
        Channel* channel = channelUsers[i].second;
        std::string formattedClient = client->getNick();
        std::string response = ARPL_PART(client->getNick(), channelsToLeave[i]);

        channel->broadcast(response);
        channel->removeUser(channelUser->client);
    }
}


void ChannelHandler::removeClientFromAllChannels(Client* client) {
    for (size_t i = 0; i < this->channels.size(); i++) {
        Channel* channel = this->channels[i];
        if (channel->getChannelUser(client) == NULL) {
            continue;
        }
        std::string formattedClient = client->getNick();
        std::string response = ARPL_PART(client->getNick(), channel->getKey());
        channel->broadcast(response);
        channel->removeUser(client);
    }
}
