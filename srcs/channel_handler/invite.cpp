
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
