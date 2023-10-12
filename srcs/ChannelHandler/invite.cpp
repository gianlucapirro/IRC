
#include "ChannelHandler.hpp"

void ChannelHandler::handleInvite(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        respond(client->getFD(), AERR_NEEDMOREPARAMS(client->getNick(), "INVITE"));
        return;
    }

    Client* invitedClient = getClientByNick(this->clients, args[0]);
    if (invitedClient == NULL) {
        respond(client->getFD(), AERR_NOSUCHNICK(client->getNick(), args[0]));
        return;
    }

    Channel* channel = this->getChannelByKey(args[1]);
    if (channel == NULL) {
        respond(client->getFD(), AERR_NOSUCHCHANNEL(client->getNick(), args[1]));
        return;
    }

    ChannelUser* channelUser = channel->getChannelUser(client);
    if (channelUser == NULL) {
        respond(client->getFD(), AERR_NOTONCHANNEL(client->getNick(), channel->getKey()));
        return;
    }

    if (channelUser->isOperator == false) {
        respond(client->getFD(), AERR_CHANOPRIVSNEEDED(client->getNick(), channel->getKey()));
        return;
    }

    ChannelUser* invitedChannelUser = channel->getChannelUser(invitedClient);
    if (invitedChannelUser != NULL) {
        respond(client->getFD(), AERR_USERONCHANNEL(client->getNick(), invitedClient->getNick(), channel->getKey()));
        return;
    }

    respond(client->getFD(), ARPL_INVITING(client->getNick(), invitedClient->getNick(), channel->getKey()));
    respond(invitedClient->getFD(), ARPL_INVITE(client->getFullClientIdentifier(), channel->getKey(), invitedClient->getNick()));
    
    channel->addInvite(invitedClient->getNick());
}