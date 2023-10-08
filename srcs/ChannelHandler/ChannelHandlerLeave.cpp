#include "ChannelHandler.hpp"


void ChannelHandler::handleLeave(Client* client, const std::vector<std::string>& args) {
    if (args.size() < 1) {
        respond(client->getFD(), AERR_NEEDMOREPARAMS(client->getNick(), "PART"));
        return;
    }
    std::string name = args[0];
    std::string reason = "Leaving";
    if (args.size() > 1) {
        reason = args[1];
        for (size_t i = 2; i < args.size(); i++) {
            reason += " ";
            reason += args[i];
        }
    }

    Channel* channel = this->getChannelByKey(name);
    if (channel == NULL) {
        respond(client->getFD(), AERR_NOSUCHCHANNEL(client->getNick(), name));
        return;
    }

    ChannelUser* channelUser = channel->getChannelUser(client);
    if (channelUser == NULL) {
        respond(client->getFD(), AERR_NOTONCHANNEL(client->getNick(), name));
        return;
    }

    channel->broadcast(ARPL_PART(client->getFullClientIdentifier(), name, reason));
    channel->removeUser(channelUser->client);
    this->removeChannelIfEmpty(channel);
}