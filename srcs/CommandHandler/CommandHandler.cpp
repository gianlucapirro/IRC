#include "CommandHandler.hpp"

CommandHandler::CommandHandler(const Config* config, std::vector<Client*>* clients) : channelHandler(clients){
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


void CommandHandler::handleCommand(Client *client, const std::string& command, const std::vector<std::string>& args) {

    if (args.empty()) {
        respond(client->getFD(), AERR_NEEDMOREPARAMS(client->getNick(), command));
        return;
    }
    if (!client->getIsRegistered()) {
        handleRegistration(client, command, args);
        return;
    }

    // Check registration
    if (command == "JOIN") {
        handleJoin(client, args);
    } else if (command == "PRIVMSG") {
        handlePrivMsg(client, args);
    } else if (command == "KICK") {
        handleKick(client, args);
    } else if (command == "PART") {
        handleLeave(client, args);
    } else if (command == "NICK") {
        handleNick(client, args);
    } else if (command == "TOPIC") {
        handleTopic(client, args);
    } else if (command == "INVITE") {
        handleInvite(client, args);
    } else if (command == "MODE") {
        handleMode(client, args);
    } else if (command == "PING") {
        handlePing(client);
    }
}

void CommandHandler::handleInvite(Client* client, const std::vector<std::string>& args) {
    this->channelHandler.handleInvite(client, args);
}

void CommandHandler::handleTopic(Client* client, const std::vector<std::string>& args) {
    this->channelHandler.handleTopic(client, args);
}

void CommandHandler::handleMode(Client* client, const std::vector<std::string>& args) {
    this->channelHandler.handleMode(client, args);
}

void CommandHandler::handleJoin(Client *client, const std::vector<std::string>& args) {
    this->channelHandler.join(client, args);
}

void CommandHandler::handleCap(Client *client, const std::vector<std::string>& args) {
    if (args[0] == "LS") {
        respond(client->getFD(), ARPL_CAP_LS());
    }
}

void CommandHandler::handlePass(Client *client, const std::vector<std::string>& args) {
    if (args[0] == this->config->getPassword()) {
        client->setIsAuthenticated(true);
    } else {
        respond(client->getFD(), AERR_PASSWDMISMATCH(client->getNick()));

        // TODO: figure out, if pass wrong. close current fd, or idle and ask try pass again?
    }
}

void CommandHandler::handleNick(Client *client, const std::vector<std::string>& args) {

    if (!client->isValidNickname(args[0])) {
        respond(client->getFD(), AERR_ERRONEUSNICKNAME(client->getNick(), args[0]));
        return;
    }

    if (this->isNicknameInUse(args[0])) {
        respond(client->getFD(), AERR_NICKNAMEINUSE(client->getNick(), args[0]));
        return;
    }

    const std::string nick = client->getNick();
    respond(client->getFD(), ARPL_NICK(client->getNick(), args[0]));

    client->setNick(args[0]);
	if (client->canBeRegistered())
		client->registerClient();
}

void CommandHandler::handleUser(Client *client, const std::vector<std::string>& args) {
    if (args.size() != 4) {
        respond(client->getFD(), AERR_NEEDMOREPARAMS(client->getNick(), "USER"));
        return;
    }

    if (!client->getIsAuthenticated()) {
        respond(client->getFD(), AERR_NOTREGISTERED(client->getNick()));
        return;
    }
    if (!client->isValidUsername(args[0])) {
        respond(client->getFD(), AERR_ERRONEUSNICKNAME(client->getNick(), args[0]));
        return;
    }

	client->setUsername(args[0]);
	client->setHostname(args[2]);
	if (client->canBeRegistered())
		client->registerClient();
}

void CommandHandler::handlePrivMsg(Client *client, const std::vector<std::string>& args) {
    std::string channelOrUser = args[0];
    if (channelOrUser[0] == '#') {
        this->channelHandler.handleMsg(client, args);
    } else {
		sendPrivateMessage(client, this->clients, args);
	}
}

void CommandHandler::handleKick(Client *client, const std::vector<std::string>& args) {
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

    this->channelHandler.handleKick(client, channelsToKick, clientsToKick, reason);
}

void CommandHandler::handleLeave(Client* client, const std::vector<std::string>& args) {
    this->channelHandler.handleLeave(client, args);
}


void CommandHandler::handlePing(Client* client) const {
    respond(client->getFD(), ARPL_PONG());
}


Client* CommandHandler::searchClient(int clientFD) {
    std::vector<Client*> clients = *(this->clients);

    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i]->getFD() == clientFD) {
            return clients[i];
        } // ja, gian het kan wel anders!
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

