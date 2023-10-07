#include "ChannelHandler.hpp"


int MINUS = 0;
int PLUS = 1;

struct ModeInfo {
    bool argOnMinus;
    bool argOnPlus;
};

char MODES[5] = {'i', 't', 'k', 'o', 'l'};
bool MODE_TABLE[5][2] = {
    {false, false},
    {false, false},
    {false, true},  
    {true, true},
    {false, true},
};

Channel* ChannelHandler::modeGetChannelOrRespond(Client* client, const std::string& name) {
    Channel* channel = this->getChannelByKey(name);
    if (channel == NULL) {
        respond(client->getFD(), AERR_NOSUCHCHANNEL(client->getNick(), name));
    }
    return channel;
}

int ChannelHandler::modeGetOperatorOrRespond(Client* client, std::string modeString) {

    int plusOrMinus = MINUS;

    if (modeString == "") {
        respond(client->getFD(), AERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
        return -1;
    } else if (modeString[0] == '-')
        plusOrMinus = MINUS; 
    else if (modeString[0] == '+')
        plusOrMinus = PLUS;
    else {
        respond(client->getFD(), AERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
        return -1;
    }

    return plusOrMinus;
}

static int getModeIndex(char c) {
    for (int i = 0; i < 5; i++) {
        if (MODES[i] == c) {
            return i;
        }
    }
    return -1;
}


std::vector<ModeChange> ChannelHandler::modeParseArgsOrRespond(Client *client, const std::vector<std::string>& args) {
    std::vector<ModeChange> changes; 

    if (args.size() < 2) {
        respond(client->getFD(), AERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
        return changes;
    }

    Channel* channel = this->modeGetChannelOrRespond(client, args[0]);
    if (channel == NULL) {
        return changes;
    }

    std::string modeString = args[1];
    int op = this->modeGetOperatorOrRespond(client, modeString);
    if (op == -1) {
        return changes;
    }

    int argCounter = 0;
    for (size_t i = 1; i < modeString.size(); i++) {
        char c = modeString[i];
        int index = getModeIndex(c);
        if (index == -1) {
            respond(client->getFD(), AERR_UNKNOWNMODE(client->getNick(), std::string(1, c)));
            changes.clear();
            return changes;
        }

        std::string arg = "";
        bool hasArg = MODE_TABLE[index][op];
        if (hasArg == true) {
            int argIndex = 2 + argCounter; // 0 and 1 index are filed with channel and modestring
            if ((size_t)argIndex >= args.size()) {
                respond(client->getFD(), AERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
                changes.clear();
                return changes;
            }
            arg = args[argIndex];
            argCounter++;
        }
        ModeChange change = {op, c, channel, arg};
        changes.push_back(change);
    }
    return changes;
}

bool ChannelHandler::modeSetInviteOnly(Client* client, ModeChange change) {
    std::string flag = change.operation == PLUS ? "+i" : "-i";
    std::string resp = ARPL_MODE(client->getNick(), change.channel->getKey(), flag, "");
    change.channel->setInviteOnly(change.operation == PLUS ? true : false);
    change.channel->broadcast(resp);
    return true;
}

bool ChannelHandler::modeSetPass(Client* client, ModeChange change) {
    if (change.operation == MINUS) {
        std::string resp = ARPL_MODE(client->getNick(), change.channel->getKey(), "-k", "");
        change.channel->setPass("");
        change.channel->broadcast(resp);
        return true;
    }
    change.channel->setPass(change.argument);
    std::string resp = ARPL_MODE(client->getNick(), change.channel->getKey(), "+k", change.argument);
    change.channel->broadcast(resp);
    return true;
}


bool ChannelHandler::modeSetTopic(Client* client, ModeChange change) {
    if (change.operation == MINUS) {
        std::string resp = ARPL_MODE(client->getNick(), change.channel->getKey(), "-t", "");
        change.channel->setCanChangeTopic(false);
        change.channel->broadcast(resp);
        return true;
    } else {
        std::string resp = ARPL_MODE(client->getNick(), change.channel->getKey(), "+t", "");
        change.channel->setCanChangeTopic(true);
        change.channel->broadcast(resp);
        return true;
    }
}

bool ChannelHandler::modeSetLimit(Client* client, ModeChange change) {
    if (change.operation == MINUS) {
        change.channel->setLimit(0);
        std::string resp = ARPL_MODE(client->getNick(), change.channel->getKey(), "-l", "");
        change.channel->broadcast(resp);
        return true;
    }
    int newLimit = 0;
    if (std::sscanf(change.argument.c_str(), "%d", &newLimit) != 1 || newLimit <= 0) {
        respond(client->getFD(), AERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
        return false;
    }
    change.channel->setLimit(newLimit);
    std::string resp = ARPL_MODE(client->getNick(), change.channel->getKey(), "+l", change.argument);
    change.channel->broadcast(resp);

    return true;
}

bool ChannelHandler::modeSetOperator(Client* client, ModeChange change) {

    Client* toPromote = getClientByNick(this->clients, change.argument);
    ChannelUser* channelUserToPromote = NULL;
    if (toPromote != NULL)
        channelUserToPromote = change.channel->getChannelUser(toPromote);
    if (channelUserToPromote == NULL) {
        respond(client->getFD(), AERR_NOSUCHNICK(client->getNick(), change.argument));
        return false;
    }

    channelUserToPromote->isOperator = change.operation == PLUS ? true : false;
    std::string flag = change.operation == PLUS ? "+o" : "-o";
    std::string operatorResponse = ARPL_MODE(client->getNick(), change.channel->getKey(), flag, toPromote->getNick());

    change.channel->broadcast(operatorResponse);
    return true;
}

static bool checkOperator(Client* client, Channel* channel) {

    ChannelUser* channelUser = channel->getChannelUser(client);
    if (channelUser == NULL) {
        respond(client->getFD(), AERR_NOTONCHANNEL(client->getNick(), channel->getKey()));
        return false;
    }

    if (channelUser->isOperator == false) {
        respond(client->getFD(), AERR_CHANOPRIVSNEEDED(client->getNick(), channel->getKey()));
        return false;
    }
    return true;
}

void ChannelHandler::handleMode(Client* client, const std::vector<std::string>& args) {
    std::vector<ModeChange> changes = this->modeParseArgsOrRespond(client, args);
    if (changes.size() == 0) {
        return;
    }
    if (checkOperator(client, changes[0].channel) == false) {
        return;
    }


    for (size_t i = 0; i < changes.size(); i++) {
        if (changes[i].mode == 'o') {
            this->modeSetOperator(client, changes[i]);
        } else if (changes[i].mode == 'l') {
            this->modeSetLimit(client, changes[i]);
        } else if (changes[i].mode == 'k') {
            this->modeSetPass(client, changes[i]);
        } else if (changes[i].mode == 'i') {
            this->modeSetInviteOnly(client, changes[i]);
        } else if (changes[i].mode == 't') {
            this->modeSetTopic(client, changes[i]);
        }
    }
}