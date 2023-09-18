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
        std::string resp = ResponseBuilder("ircserv").addCommand(ERR_NOSUCHCHANNEL).addParameters(name).addTrailing("This channel does not exist").build();
        this->messageQueue->push(std::make_pair(client->getFD(), resp));
    }
    return channel;
}

int ChannelHandler::modeGetOperatorOrRespond(Client* client, std::string modeString) {

    int plusOrMinus = MINUS;

    if (modeString == "") {
        std::string resp = ResponseBuilder("ircserv").addCommand(ERR_NEEDMOREPARAMS).addParameters("MODE").addTrailing("Need a '-' or '+'").build();
        messageQueue->push(std::make_pair(client->getFD(), resp));
        return -1;
    } else if (modeString[0] == '-')
        plusOrMinus = MINUS; 
    else if (modeString[0] == '+')
        plusOrMinus = PLUS;
    else {
        std::string resp = ResponseBuilder("ircserv").addCommand(ERR_NEEDMOREPARAMS).addParameters("MODE").addTrailing("Need a '-' or '+'").build();
        messageQueue->push(std::make_pair(client->getFD(), resp));
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
        std::string resp = ResponseBuilder("ircserv").addCommand(ERR_NEEDMOREPARAMS).addParameters("MODE").addTrailing("Need at least 2 parameters").build();
        messageQueue->push(std::make_pair(client->getFD(), resp));
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
            std::string resp = ResponseBuilder("ircserv").addCommand(ERR_UNKNOWNMODE).addParameters(std::string(1, c)).addTrailing("We don't support this mode").build();
            messageQueue->push(std::make_pair(client->getFD(), resp));
            changes.clear();
            return changes;
        }

        std::string arg = "";
        bool hasArg = MODE_TABLE[index][op];
        if (hasArg == true) {
            int argIndex = 2 + argCounter; // 0 and 1 index are filed with channel and modestring
            if ((size_t)argIndex >= args.size()) {
                std::string resp = ResponseBuilder("ircserv").addCommand(ERR_NEEDMOREPARAMS).addParameters("MODE").addTrailing("No parameter provided for '" + std::string(1, c) + "'").build();
                messageQueue->push(std::make_pair(client->getFD(), resp));
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
    std::string flag = change.operation == PLUS ? " +i" : " -i";
    std::string resp = ResponseBuilder(client->getNick()).addCommand("MODE").addParameters(change.channel->getKey() + flag).build();
    change.channel->setInviteOnly(change.operation == PLUS ? true : false);
    change.channel->broadcast(resp, this->messageQueue);
    return true;
}

bool ChannelHandler::modeSetPass(Client* client, ModeChange change) {
    if (change.operation == MINUS) {
        std::string resp = ResponseBuilder(client->getNick()
            ).addCommand("MODE"
            ).addParameters(change.channel->getKey() + " -k " + change.channel->getPass() 
            ).build();
        change.channel->setPass("");
        change.channel->broadcast(resp, this->messageQueue);
        return true;
    }
    change.channel->setPass(change.argument);
    std::string resp = ResponseBuilder(client->getNick()
        ).addCommand("MODE"
        ).addParameters(change.channel->getKey() + " +k " + change.argument
        ).build();
    change.channel->broadcast(resp, this->messageQueue);
    return true;
}


bool ChannelHandler::modeSetTopic(Client* client, ModeChange change) {
    if (change.operation == MINUS) {
        std::string resp = ResponseBuilder(client->getNick()
            ).addCommand("MODE"
            ).addParameters(change.channel->getKey() + " -t"
            ).build();
        change.channel->setCanChangeTopic(false);
        change.channel->broadcast(resp, this->messageQueue);
        return true;
    } else {
        std::string resp = ResponseBuilder(client->getNick()
            ).addCommand("MODE"
            ).addParameters(change.channel->getKey() + " +t"
            ).build();
        change.channel->setCanChangeTopic(true);
        change.channel->broadcast(resp, this->messageQueue);
        return true;
    }
}

bool ChannelHandler::modeSetLimit(Client* client, ModeChange change) {
    if (change.operation == MINUS) {
        change.channel->setLimit(0);
        std::string resp = ResponseBuilder(client->getNick()
            ).addCommand("MODE"
            ).addParameters(change.channel->getKey() + " -l" 
            ).build();
        change.channel->broadcast(resp, this->messageQueue);
        return true;
    }
    int newLimit = 0;
    if (std::sscanf(change.argument.c_str(), "%d", &newLimit) != 1 || newLimit <= 0) {
        std::string errMsg = "Please give a valid number for limit argument";
        std::string resp = ResponseBuilder("ircserv").addCommand(ERR_NEEDMOREPARAMS).addParameters("l").addTrailing(errMsg).build();
        messageQueue->push(std::make_pair(client->getFD(), resp));
        return false;
    }
    change.channel->setLimit(newLimit);
    std::string resp = ResponseBuilder(client->getNick()
        ).addCommand("MODE"
        ).addParameters(change.channel->getKey() + " +l " + change.argument
        ).build();
    change.channel->broadcast(resp, this->messageQueue);

    return true;
}

bool ChannelHandler::modeSetOperator(Client* client, ModeChange change) {

    Client* toPromote = getClientByNick(this->clients, change.argument);
    ChannelUser* channelUserToPromote = NULL;
    if (toPromote != NULL)
        channelUserToPromote = change.channel->getChannelUser(toPromote);
    if (channelUserToPromote == NULL) {
        std::string resp = ResponseBuilder("ircserv").addCommand(ERR_NOSUCHNICK).addParameters(change.argument).addTrailing("This nickname is not on this channel").build();
        messageQueue->push(std::make_pair(client->getFD(), resp));
        return false;
    }

    channelUserToPromote->isOperator = change.operation == PLUS ? true : false;
    std::string operatorResponse = ResponseBuilder(client->getNick()
        ).addCommand("MODE"
        ).addParameters(change.channel->getKey() + " -o " + toPromote->getNick()
        ).build();

    change.channel->broadcast(operatorResponse, this->messageQueue);
    return true;
}

static bool checkOperator(Client* client, Channel* channel, std::queue<message>* messageQueue) {

    ChannelUser* channelUser = channel->getChannelUser(client);
    if (channelUser == NULL) {
        std::string resp = ResponseBuilder("ircserv").addCommand(ERR_NOTONCHANNEL).addParameters(channel->getKey()).addTrailing("You're not on this channel").build();
        messageQueue->push(std::make_pair(client->getFD(), resp));
        return false;
    }

    if (channelUser->isOperator == false) {
        std::string resp = ResponseBuilder("ircserv").addCommand(ERR_CHANOPRIVSNEEDED).addParameters(channel->getKey()).addTrailing("You're not a channel operator").build();
        messageQueue->push(std::make_pair(client->getFD(), resp));
        return false;
    }
    return true;
}

void ChannelHandler::handleMode(Client* client, const std::vector<std::string>& args) {
    std::vector<ModeChange> changes = this->modeParseArgsOrRespond(client, args);
    if (changes.size() == 0) {
        return;
    }
    if (checkOperator(client, changes[0].channel, this->messageQueue) == false) {
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