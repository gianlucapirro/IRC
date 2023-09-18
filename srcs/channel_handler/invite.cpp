
#include "ChannelHandler.hpp"

ChannelHandler::ChannelHandler(std::queue<message>* messageQueue, std::vector<Client*> *clients) {
    this->messageQueue = messageQueue;
    this->clients = clients;
}

void ChannelHandler::join(Client* client, const std::vector<std::string>& args, std::queue<message> *messageQueue) {
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

    channel->broadcast(ARPL_JOIN(client->getNick(), channel->getKey()), messageQueue);

    // TODO: add topic if it exists
    respond(client->getFD(), ARPL_NOTOPIC(client->getNick(), channel->getKey()));

    std::string names = channel->getNicknames();
    respond(client->getFD(), ARPL_NAMREPLY(client->getNick(), channel->getKey(), names));
    respond(client->getFD(), ARPL_ENDOFNAMES(client->getNick(), channel->getKey()));
}

void ChannelHandler::handleMsg(Client* client, const std::vector<std::string>& args, std::queue<message> *messageQueue) {
    // TODO: avoid sending to channel that you're not in
    Channel *channel = this->getChannelByKey(args[0]);
    if (channel != NULL) {
        channel->sendMsg(client, args, messageQueue);
    }
}

void ChannelHandler::handleKick(
    Client* client,
    const std::vector<std::string>& channelsToKick,
    const std::vector<std::string>& clientsToKick,
    std::vector<Client*> *clients,
    std::queue<message> *messageQueue,
    std::string reason
) {
    std::vector<Channel*> foundChannels;
    for (size_t i = 0; i < channelsToKick.size(); i++) {
        Channel* foundChannel = this->getChannelByKey(channelsToKick[i]);
        if (foundChannel == NULL) {
            std::string response = ResponseBuilder("ircserv").addCommand("403").addParameters(channelsToKick[i]).addTrailing("No such channel").build();
            messageQueue->push(std::make_pair(client->getFD(), response));
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
            std::string response = ResponseBuilder("ircserv").addCommand("KICK").addParameters(foundChannels[i]->getKey() + " " + foundClients[p]->getNick()).addTrailing(reason).build();
            messageQueue->push(std::make_pair(foundClients[p]->getFD(), response));
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
        std::string response = ResponseBuilder("ircserv").addCommand(ERR_NEEDMOREPARAMS).addTrailing("Need more parameters").build();
        messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    Channel* foundChannel = this->getChannelByKey(args[0]);
    if (foundChannel == NULL) {
        std::string response = ResponseBuilder("ircserv").addCommand(ERR_NOSUCHCHANNEL).addParameters(args[0]).addTrailing("No such channel").build();
        messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    ChannelUser* channelUser = foundChannel->getChannelUser(client);
    if (channelUser == NULL) {
        std::string response = ResponseBuilder("ircserv").addCommand(ERR_NOTONCHANNEL).addParameters(foundChannel->getKey()).addTrailing("You're not on that channel").build();
        messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    if (args.size() == 1) {
        if (foundChannel->getTopic() == "") {
            std::string response = ResponseBuilder("ircserv").addCommand(RPL_NOTOPIC).addParameters(foundChannel->getKey()).addTrailing("No topic is set").build();
            messageQueue->push(std::make_pair(client->getFD(), response));
        } else {
            std::string response = ResponseBuilder("ircserv").addCommand(RPL_TOPIC).addParameters(foundChannel->getKey()).addTrailing(foundChannel->getTopic()).build();
            messageQueue->push(std::make_pair(client->getFD(), response));
        }
        return;
    }
    if (foundChannel->getCanChangeTopic() == false && channelUser->isOperator == false) {
        std::string response = ResponseBuilder("ircserv").addCommand(ERR_CHANOPRIVSNEEDED).addParameters(foundChannel->getKey()).addTrailing("You're not a channel operator").build();
        messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }
    foundChannel->setTopic(args[1]);
    std::string response = ResponseBuilder("ircserv").addCommand("TOPIC").addParameters(foundChannel->getKey()).addTrailing(foundChannel->getTopic()).build();
    foundChannel->broadcast(response, messageQueue);
    return;
}

void ChannelHandler::handleLeave(Client* client, const std::vector<std::string>& channelsToLeave, std::queue<message> *messageQueue) {

    std::vector<Channel*> channels;

    for (size_t i = 0; i < channelsToLeave.size(); i++) {

        Channel* channel = this->getChannelByKey(channelsToLeave[i]);
        if (channel == NULL) {
            std::string response = ResponseBuilder("ircserv").addCommand("403").addParameters(channelsToLeave[i]).addTrailing("No such channel").build();
            messageQueue->push(std::make_pair(client->getFD(), response));
            return;
        }
        channels.push_back(channel);
    }

    std::vector<std::pair<ChannelUser*, Channel*> > channelUsers;

    for (size_t i = 0; i < channels.size(); i++) {
        Channel* channel = channels[i];
        ChannelUser* channelUser = channel->getChannelUser(client);
        if (channelUser == NULL) {
            std::string response = ResponseBuilder("ircserv").addCommand("442").addParameters(channelsToLeave[i]).addTrailing("You're not on that channel").build();
            messageQueue->push(std::make_pair(client->getFD(), response));
            return;
        }
        channelUsers.push_back(std::make_pair(channelUser, channel));
    }

    for (size_t i = 0; i < channelUsers.size(); i++) {
        ChannelUser* channelUser = channelUsers[i].first;
        Channel* channel = channelUsers[i].second;
        std::string formattedClient = client->getNick();
        std::string response = ResponseBuilder(formattedClient).addCommand("PART").addParameters(channelsToLeave[i]).addTrailing("").build();

        channel->broadcast(response, messageQueue);
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
        std::string response = ResponseBuilder(formattedClient).addCommand("PART").addParameters(channel->getKey()).addTrailing("").build();
        channel->broadcast(response, messageQueue);
        channel->removeUser(client);
    }
}


void ChannelHandler::handleInvite(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::string response = ResponseBuilder("ircserv").addCommand(ERR_NEEDMOREPARAMS).addTrailing("Need more parameters").build();
        messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    Client* invitedClient = getClientByNick(this->clients, args[0]);
    if (invitedClient == NULL) {
        std::string response = ResponseBuilder("ircserv").addCommand(ERR_NOSUCHNICK).addParameters(client->getNick() + " " + args[0]).addTrailing("No such nickname").build();
        messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    Channel* channel = this->getChannelByKey(args[1]);
    if (channel == NULL) {
        std::string response = ResponseBuilder("ircserv").addCommand(ERR_NOTONCHANNEL).addParameters(args[1]).addTrailing("No such channel").build();
        messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    ChannelUser* channelUser = channel->getChannelUser(client);
    if (channelUser == NULL) {
        std::string response = ResponseBuilder("ircserv"
            ).addCommand(ERR_NOTONCHANNEL
            ).addParameters(client->getNick() + " " + channel->getKey()
            ).addTrailing("You are not on this channel"
            ).build();
        messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    if (channelUser->isOperator == false) {
        std::string response = ResponseBuilder("ircserv"
            ).addCommand(ERR_CHANOPRIVSNEEDED
            ).addParameters(client->getNick() + " " + channel->getKey()
            ).addTrailing("You are not a channel operator"
            ).build();
        messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    ChannelUser* invitedChannelUser = channel->getChannelUser(invitedClient);
    if (invitedChannelUser != NULL) {
        std::string response = ResponseBuilder("ircserv"
            ).addCommand(ERR_USERONCHANNEL
            ).addParameters(client->getNick() + " " + invitedClient->getNick() + " " + channel->getKey()
            ).addTrailing("This user is already on this channel"
            ).build();
        messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    std::string inviterResponse = ResponseBuilder("ircserv"
        ).addCommand(RPL_INVITING
        ).addParameters(client->getNick() + " " + invitedClient->getNick() + " " + channel->getKey()
        ).build();
    messageQueue->push(std::make_pair(client->getFD(), inviterResponse));

    std::string inviteeResponse = ResponseBuilder(client->getNick()
        ).addCommand("INVITE"
        ).addParameters(invitedClient->getNick()
        ).addTrailing(channel->getKey()
        ).build();
    messageQueue->push(std::make_pair(invitedClient->getFD(), inviteeResponse));
    channel->addInvite(invitedClient->getNick());
}
