#include "ChannelHandler.hpp"

static bool isValidChannelName(std::string name) {
    if (name[0] != '#') {
        return false;
    }
    for (size_t i = 1; i < name.length(); i++) {
        if (!isalnum(name[i])) {
            return false;
        }
    }
    return true;
}

static bool userInChannel(Client* client, Channel* channel) {
    ChannelUser* channelUser = channel->getChannelUser(client);
    if (channelUser == NULL) {
        return false;
    }
    return true;
}

static bool canUserJoinChannel(Client* client, Channel* channel, const std::vector<std::string>& args) {

    if (channel->getLimit() != 0 && channel->getLimit() <= channel->getUserCount()) {
        respond(client->getFD(), AERR_CHANNELISFULL(client->getNick(), channel->getKey()));
        return false;
    }
    
    if (channel->getPass() != "" && (args.size() < 2 || args[1] != channel->getPass())) {
        respond(client->getFD(), AERR_BADCHANNELKEY(client->getNick(), channel->getKey()));
        return false;
    }
    
    if (channel->getIsInviteOnly() && !channel->isInvited(client->getNick())) {
        respond(client->getFD(), AERR_INVITEONLYCHAN(client->getNick(), channel->getKey()));
        return false;
    }

    return true;
}

static void sendJoinResponse(Client* client, Channel* channel) {
    channel->broadcast(ARPL_JOIN(client->getFullClientIdentifier(), channel->getKey()));
    std::string names = channel->getNicknames();
    std::string topic = channel->getTopic();
    if (topic != "") {
        respond(client->getFD(), ARPL_TOPIC(client->getNick(), channel->getKey(), topic));
    } else {
        respond(client->getFD(), ARPL_NOTOPIC(client->getNick(), channel->getKey()));
    }
    respond(client->getFD(), ARPL_NAMREPLY(client->getNick(), channel->getKey(), names));
    respond(client->getFD(), ARPL_ENDOFNAMES(client->getNick(), channel->getKey()));
}


Channel* ChannelHandler::getOrCreateChannel(Client* client, const std::vector<std::string>& args) {
    Channel *channel = this->getChannelByKey(args[0]);

    if (!channel && !isValidChannelName(args[0])) {
        respond(client->getFD(), AERR_NOSUCHCHANNEL(client->getNick(), args[0]));
        return NULL;
    }
    if (!channel) {
        channel = new Channel(client, args[0]);
        this->channels.push_back(channel);
    }
    return channel;
}


void ChannelHandler::join(Client* client, const std::vector<std::string>& args) {
    Channel* channel = this->getOrCreateChannel(client, args);
    if (channel == NULL)
        return;
    
    if (userInChannel(client, channel) == false) {
        if (canUserJoinChannel(client, channel, args) == false)
            return;
        channel->addUser(client, false);
    }
    sendJoinResponse(client, channel);
}
