#include "CommandHandler.hpp"

#include <sstream>

#include "ResponseBuilder.hpp"
#include "Server.hpp"

CommandHandler::CommandHandler(Server* server) : server(server) {
    this->channelHandler = ChannelHandler();
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

std::vector<parsedCommand> CommandHandler::parseCommands(const std::string& lines) {
    std::vector<parsedCommand> parsedCommands;
    std::istringstream ss(lines);
    std::string line;

    // split the input by newline
    while (std::getline(ss, line)) {
        // remove carriage return character at the end if it exists
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }

        parsedCommand parsedCommand = parseCommand(line);  // parse individual command
        parsedCommands.push_back(parsedCommand);
    }

    return parsedCommands;
}

void CommandHandler::handleCommand(int clientFD, const std::string& command, const std::vector<std::string>& args) {
    // std::cout << "========================" << std::endl;
    // std::cout << "Command: " << command << std::endl;
    // for (std::vector<std::string>::const_iterator i = args.begin(); i != args.end(); ++i) {
    //     std::cout << *i << std::endl;
    // }
    // std::cout << "========================" << std::endl;

    if (args.empty()) {
        std::string response = ResponseBuilder("ircserv").addCommand("461").addTrailing("Not enough parameters").build();
        this->server->sendMessage(clientFD, response);
        return;
    }
    if (command == "PASS") {
        handlePass(clientFD, args);
    } else if (command == "NICK") {
        handleNick(clientFD, args);
    } else if (command == "USER") {
        handleUser(clientFD, args);
    }
}

void CommandHandler::handleCap(int clientFD, const std::vector<std::string>& args) {
    if (args[0] == "LS") {
        std::string capabilities = "";
        std::string response =
            ResponseBuilder("ircserv").addCommand("CAP").addParameters("* LS :" + capabilities).build();
        this->server->sendMessage(clientFD, response);
    }
}

void CommandHandler::handlePass(int clientFD, const std::vector<std::string>& args) {
    if (args[0] == this->server->getConfig().getPassword()) {
        Client* client = this->server->searchClient(clientFD);
        client->setIsAuthenticated(true);
    } else {
        std::cout << "wrong password passed: " << args[0] << std::endl;
        std::string response = ResponseBuilder("ircserv").addCommand("464").addTrailing("Password incorrect").build();
        this->server->sendMessage(clientFD, response);
        // TODO: figure out, if pass wrong. close current fd, or idle and ask try pass again?
    }
}

void CommandHandler::handleNick(int clientFD, const std::vector<std::string>& args) {
    Client* client = this->server->searchClient(clientFD);
    if (client == NULL || !client->getIsAuthenticated()) {
        std::string message =
            ResponseBuilder("ircserv").addCommand("451").addTrailing("You have not registered").build();
        this->server->sendMessage(clientFD, message);
        return;
    }

    if (!client->isValidNickname(args[0])) {
        std::string response = ResponseBuilder("ircserv")
                                   .addCommand("432")
                                   .addParameters("* " + args[0])
                                   .addTrailing("Erroneous nickname")
                                   .build();
        this->server->sendMessage(client->getFD(), response);
        return;
    }

    if (this->server->isNicknameInUse(args[0])) {
        std::string response = ResponseBuilder("ircserv")
                                   .addCommand("433")
                                   .addParameters("* " + args[0])
                                   .addTrailing("Nickname is already in use")
                                   .build();
        this->server->sendMessage(client->getFD(), response);
        return;
    }

    client->setNick(args[0]);
    std::string response =
        ResponseBuilder(client->getNick()).addCommand("NICK").addParameters(client->getNick()).build();
    this->server->sendMessage(client->getFD(), response);
}

void CommandHandler::handleUser(int clientFD, const std::vector<std::string>& args) {
    if (args.size() != 4) {
        std::string response =
            ResponseBuilder("ircserv").addCommand("461").addTrailing("Not enough parameters").build();
        this->server->sendMessage(clientFD, response);
        return;
    }

    Client* client = this->server->searchClient(clientFD);
    if (client == NULL || !client->getIsAuthenticated()) {
        std::string message =
            ResponseBuilder("ircserv").addCommand("451").addTrailing("You have not registered").build();
        this->server->sendMessage(clientFD, message);
        return;
    }
    if (!client->isValidUsername(args[0])) {
        std::string response = ResponseBuilder("ircserv")
                                   .addCommand("432")
                                   .addParameters("* " + args[0])
                                   .addTrailing("Erroneous username")
                                   .build();
        this->server->sendMessage(client->getFD(), response);
        return;
    }

    client->setUsername(args[0]);
    client->setHostname(args[2]);

    std::string welcomeMsg =
        ResponseBuilder("ircserv")
            .addCommand("001")
            .addParameters(client->getNick())
            .addTrailing("Welcome to the Internet Relay Network " + client->getFullClientIdentifier())
            .build();

    this->server->sendMessage(clientFD, welcomeMsg);
}
