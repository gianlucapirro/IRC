#include "ChannelHandler.hpp"


int MINUS = 0;
int PLUS = 1;

struct ModeInfo {
    bool argOnMinus;
    bool argOnPlus;
};

std::unordered_map<char, std::array<int, 2> > MODE_TABLE = {
    {'i', {false, false}},
    {'t', {false, false}},
    {'k', {false, true}},
    {'o', {true, true}},
    {'l', {false, true}},
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
    for (int i = 1; i < modeString.size(); i++) {
        char c = modeString[i];
        if (MODE_TABLE.find(c) == MODE_TABLE.end()) {
            std::string resp = ResponseBuilder("ircserv").addCommand(ERR_UNKNOWNMODE).addParameters(std::string(1, c)).addTrailing("We don't support this mode").build();
            messageQueue->push(std::make_pair(client->getFD(), resp));
            changes.clear();
            return changes;
        }
        std::string arg = "";
        bool hasArg = MODE_TABLE[c][op];
        if (hasArg == true) {
            int argIndex = 1 + argCounter; // 0 and 1 index are filed with channel and modestring
            if (args.size() >= argIndex) {
                std::string resp = ResponseBuilder("ircserv").addCommand(ERR_NEEDMOREPARAMS).addParameters("MODE").addTrailing("No parameter provided for '" + std::string(1, c) + "'").build();
                messageQueue->push(std::make_pair(client->getFD(), resp));
                changes.clear();
                return changes;
            }
            arg = args[argIndex];
            argCounter += 1;
        }
        changes.push_back({op, c, arg});
    }
    return changes;
}

void ChannelHandler::handleMode(Client* client, const std::vector<std::string>& args, std::queue<message> *messageQueue) {
    std::vector<ModeChange> changes;
    


}