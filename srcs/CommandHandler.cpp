
#include "CommandHandler.hpp"

#include <sstream>

#include "ResponseBuilder.hpp"
#include "Server.hpp"

CommandHandler::CommandHandler(Server* server) : server(server) {}

std::pair<std::string, std::vector<std::string> > CommandHandler::parseCommand(const std::string& line) {
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

std::vector<std::pair<std::string, std::vector<std::string> > > CommandHandler::parseCommands(
    const std::string& lines) {
    std::vector<std::pair<std::string, std::vector<std::string> > > parsedCommands;
    std::istringstream ss(lines);
    std::string line;

    // split the input by newline
    while (std::getline(ss, line)) {
        // remove carriage return character at the end if it exists
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }

        std::pair<std::string, std::vector<std::string> > parsedCommand =
            parseCommand(line);  // parse individual command
        parsedCommands.push_back(parsedCommand);
    }

    return parsedCommands;
}

void CommandHandler::handleCommand(int clientFD, const std::string& command, const std::vector<std::string>& args) {
    if (command == "PASS") {
        handlePass(clientFD, args);
    } else if (command == "NICK") {
        handleNick(clientFD, args);
    }
}

void CommandHandler::handlePass(int clientFD, const std::vector<std::string>& args) {
    if (args.size() <= 0) return;
    if (args[0] == this->server->getConfig().getPassword()) {
        pollfd newFD;
        newFD.fd = clientFD;
        newFD.events = POLLIN;
        newFD.revents = 0;
        this->server->getFDS().push_back(newFD);
        Client newClient(clientFD);
        this->server->getClients().push_back(newClient);
        std::cout << "Client Created" << std::endl;
        std::string welcomeMsg = ":ircserv 001 " + newClient.getNick() + " :Welcome to the Internet Relay Network " +
                                 newClient.getNick() + "!user@localhost";

        this->server->sendMessage(clientFD, welcomeMsg);
    } else {
        std::string errorMsg = ":ircserv 464 :Password incorrect";
        this->server->sendMessage(clientFD, errorMsg);
    }
}

void CommandHandler::handleNick(int clientFD, const std::vector<std::string>& args) {
    if (args.size() <= 0) {
        std::string response = ResponseBuilder("ircserv").addCommand("431").addTrailing("No nickname given").build();
        this->server->sendMessage(clientFD, response);
        return;
    }

    Client& client = this->server->searchClient(clientFD);
    if (!client.isValidNick(args[0])) {
        std::string response = ResponseBuilder("ircserv")
                                   .addCommand("432")
                                   .addParameters("* " + args[0])
                                   .addTrailing("Erroneous nickname")
                                   .build();
        this->server->sendMessage(clientFD, response);
        return;
    }

    if (this->server->isNicknameInUse(args[0])) {
        std::string response = ResponseBuilder("ircserv")
                                   .addCommand("433")
                                   .addParameters("* " + args[0])
                                   .addTrailing("Nickname is already in use")
                                   .build();
        this->server->sendMessage(clientFD, response);
        return;
    }

    client.setNick(args[0]);
    std::cout << "Clients nickname: " << client.getNick() << std::endl;

    std::string response = ResponseBuilder(client.getNick()).addCommand("NICK").addParameters(client.getNick()).build();
    this->server->sendMessage(clientFD, response);
}
