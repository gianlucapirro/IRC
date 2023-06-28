#include "CommandHandler.hpp"

#include <sstream>

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

void CommandHandler::handleCommand(int clientFD, const std::string& command, const std::vector<std::string>& args) {
    if (command == "PASS") {
        handlePass(clientFD, args);
    } else if (command == "NICK") {
        handleNick(clientFD, args);
    } else if (command == "CAP") {
        handleCap(clientFD, args);
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
        this->server->getClients().push_back(Client(clientFD));
        std::cout << "Client Created" << std::endl;
    }
}

void CommandHandler::handleNick(int clientFD, const std::vector<std::string>& args) {
    (void)clientFD;
    (void)args;
}

void CommandHandler::handleCap(int clientFD, const std::vector<std::string>& args) {
    if (args[0] == "LS") {
        this->server->sendMessage(clientFD, ":ircserv CAP * LS :r\n");
    }
    std::cout << "========================" << std::endl;
    for (std::vector<std::string>::const_iterator i = args.begin(); i != args.end(); ++i) {
        std::cout << "i = " << *i << std::endl;
    }
    std::cout << "========================" << std::endl;
}