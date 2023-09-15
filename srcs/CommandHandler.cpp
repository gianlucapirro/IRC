#include "CommandHandler.hpp"
#include "utils.hpp"

CommandHandler::CommandHandler(std::queue<message>* messages, const Config* config, std::vector<Client*>* clients) {
    this->channelHandler = ChannelHandler();
    this->messageQueue = messages;
    this->config = config;
    this->clients = clients;
}

parsedCommand CommandHandler::parseCommand(const std::string& line) {
    std::istringstream iss(line);
    std::string command;
    std::vector<std::string> args;
    std::string arg;

    iss >> command;  // Read the command part of the line

    // Read the arguments
    while (iss >> arg) {
        args.push_back(arg);
    }

    return std::make_pair(command, args);
}

std::vector<parsedCommand> CommandHandler::parseCommands(std::vector<std::string> commands) {
    std::vector<parsedCommand> parsedCommands;
    std::string command;

    for (size_t i = 0; i < commands.size(); i++) {
        command = commands[i];

        // remove carriage return character at the end if it exists
        if (!command.empty() && command[command.size() - 1] == '\r') {
            command.erase(command.size() - 1);
        }

        parsedCommand parsedCommand = parseCommand(command);  // parse individual command
        parsedCommands.push_back(parsedCommand);
    }
    return parsedCommands;
}

int CommandHandler::handleCommand(Client *client, const std::string& command, const std::vector<std::string>& args) {

    if (args.empty()) {
        std::string response =
            ResponseBuilder("ircserv").addCommand("461").addTrailing("Not enough parameters").build();

        this->messageQueue->push(std::make_pair(client->getFD(), response));
        return NO_ACTION;
    }
    if (command == "PASS") {
        handlePass(client, args);
    } else if (command == "NICK") {
        handleNick(client, args);
    } else if (command == "USER") {
        handleUser(client, args);
    } else if (command == "JOIN") {
        this->channelHandler.join(client, args, this->messageQueue);
    } else if (command == "PRIVMSG") {
        handlePrivMsg(client, args);
    } else if (command == "KICK") {
        handleKick(client, args);
    } else if (command == "QUIT") {
        return QUIT_USER;
    } else if (command == "PART") {
        handleLeave(client, args);
    }
    return NO_ACTION;
}

void CommandHandler::handleCap(Client *client, const std::vector<std::string>& args) {
    if (args[0] == "LS") {
        std::string capabilities = "";
        std::string response =
            ResponseBuilder("ircserv").addCommand("CAP").addParameters("* LS :" + capabilities).build();
        this->messageQueue->push(std::make_pair(client->getFD(), response));
    }
}

void CommandHandler::handlePass(Client *client, const std::vector<std::string>& args) {
    if (args[0] == this->config->getPassword()) {
        client->setIsAuthenticated(true);
    } else {
        std::string response = ResponseBuilder("ircserv").addCommand("464").addParameters("").addTrailing("Password incorrect").build();
        this->messageQueue->push(std::make_pair(client->getFD(), response));
        // TODO: figure out, if pass wrong. close current fd, or idle and ask try pass again?
    }
}

void CommandHandler::handleNick(Client *client, const std::vector<std::string>& args) {
    if (!client->getIsAuthenticated()) {
        std::string response =
            ResponseBuilder("ircserv").addCommand("451").addParameters("").addTrailing("You have not registered").build();
        this->messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    if (!client->isValidNickname(args[0])) {
        std::string response = ResponseBuilder("ircserv")
                                   .addCommand("432")
                                   .addParameters("* " + args[0])
                                   .addTrailing("Erroneous nickname")
                                   .build();
        this->messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    if (this->isNicknameInUse(args[0])) {
        std::string response = ResponseBuilder("ircserv")
                                   .addCommand("433")
                                   .addParameters("* " + args[0])
                                   .addTrailing("Nickname is already in use")
                                   .build();
        this->messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    const std::string nick = client->getNick();
    std::string response = ResponseBuilder(nick).addCommand("NICK").addParameters(args[0]).build();
    client->setNick(args[0]);
    this->messageQueue->push(std::make_pair(client->getFD(), response));
	if (client->canBeRegistered())
		client->registerClient(this->messageQueue);
}

void CommandHandler::handleUser(Client *client, const std::vector<std::string>& args) {
    if (args.size() != 4) {
        std::string response =
            ResponseBuilder("ircserv").addCommand("461").addTrailing("Not enough parameters").build();
        this->messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    if (!client->getIsAuthenticated()) {
        std::string response =
            ResponseBuilder("ircserv").addCommand("451").addParameters("").addTrailing("You have not registered").build();
        this->messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }
    if (!client->isValidUsername(args[0])) {
        std::string response = ResponseBuilder("ircserv")
                                   .addCommand("432")
                                   .addParameters("* " + args[0])
                                   .addTrailing("Erroneous username")
                                   .build();
        this->messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

	client->setUsername(args[0]);
	client->setHostname(args[2]);
	if (client->canBeRegistered())
		client->registerClient(this->messageQueue);
}

void CommandHandler::handlePrivMsg(Client *client, const std::vector<std::string>& args) {
    if (!client->getIsRegistered()) {
        std::string response = ResponseBuilder("ircserv").addCommand("451").addTrailing("You have not registered").build();
        this->messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    std::string channelOrUser = args[0];
    if (channelOrUser[0] == '#') {
        this->channelHandler.handleMsg(client, args, this->messageQueue);
    } else {
		sendPrivateMessage(client, this->clients, args, this->messageQueue);
	}
}

void CommandHandler::handleKick(Client *client, const std::vector<std::string>& args) {
    if (!client->getIsRegistered()) {
        std::string response = ResponseBuilder("ircserv").addCommand("451").addTrailing("You have not registered").build();
        this->messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }

    std::vector<std::string> channelsToKick;
    std::vector<std::string> clientsToKick;
    std::string reason = "You have been kicked";

    size_t i;
    for (i = 0; i < args.size() && args[i][0] == '#'; i++) {
        channelsToKick.push_back(args[i]);
    }
    for (; i < args.size() && args[i] != ":"; i++) {
        clientsToKick.push_back(args[i]);
    }
    if (i + 1 < args.size() && args[i+1] != ":") {
        reason = args[i+1];
    }


    this->channelHandler.handleKick(client, channelsToKick, clientsToKick, this->clients, this->messageQueue, reason);
}

void CommandHandler::handleLeave(Client* client, const std::vector<std::string>& args) {
    if (!client->getIsRegistered()) {
        std::string response = ResponseBuilder("ircserv").addCommand("451").addTrailing("You have not registered").build();
        this->messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }
    if (args.size() < 1) {
        std::string response = ResponseBuilder("ircserv").addCommand("461").addTrailing("Need more parameters").build();
        this->messageQueue->push(std::make_pair(client->getFD(), response));
        return;
    }
    std::vector<std::string> channelsToLeave = splitString(args[0], ',');
    this->channelHandler.handleLeave(client, channelsToLeave, this->messageQueue);
}


Client* CommandHandler::searchClient(int clientFD) {
    for (std::vector<Client*>::iterator it = this->clients->begin(); it != this->clients->end(); ++it) {
        if ((*it)->getFD() == clientFD) {
            return *it;  // nee jort het kan niet anders, dat dacht jij
        }
    }
    return NULL;
}

bool CommandHandler::isNicknameInUse(const std::string& nickname) const {
    for (size_t i = 0; i < this->clients->size(); ++i) {
        if ((*this->clients)[i]->getNick() == nickname) {
            return true;
        }
    }
    return false;
}

// void CommandHandler::handleBuffer(char* buffer, int valread, int fd) {
//     buffer[valread] = '\0';
//     std::vector<parsedCommand> parsedCommands = this->parseCommands(buffer);
//     for (std::vector<parsedCommand>::iterator it = parsedCommands.begin(); it != parsedCommands.end(); ++it) {
//         this->handleCommand(fd, it->first, it->second);
//     }
// }