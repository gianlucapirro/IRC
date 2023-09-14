#include "Server.hpp"

void Server::sendMessage(int clientFD, const std::string& message) {
    // std::cout << message.length() << ": Message send: " << message << std::endl;
    send(clientFD, message.c_str(), message.length(), 0);
}

void Server::handleBuffer(std::vector<std::string> commands, Client* client) {
    std::vector<parsedCommand> parsedCommands = this->commandHandler.parseCommands(commands);
    for (std::vector<parsedCommand>::iterator it = parsedCommands.begin(); it != parsedCommands.end(); ++it) {
        this->commandHandler.handleCommand(client, it->first, it->second);
    }
}

void Server::acceptNewConnection() {
    int newSocket;
    socklen_t addrlen = sizeof(this->address);
    if ((newSocket = accept(this->serverFD, (struct sockaddr*)&(this->address), &addrlen)) < 0) {
        throw std::runtime_error("Server Error: New connection not accepted");
    	exit(EXIT_FAILURE);
    }

    // adding new connection to fds
    this->createClient(newSocket);
}

void Server::handleClient(size_t i) {
    Client *client = this->clients[i];
    char buffer[BUFFER_SIZE];
    ssize_t valread = read(this->fds[i].fd, buffer, BUFFER_SIZE);
	// std::cout << buffer << std::endl;

    if (valread <= 0) {
        close(this->fds[i].fd);
        this->fds.erase(this->fds.begin() + i);
        this->clients.erase(this->clients.begin() + i);
        delete client;
    } else {
        std::vector<std::string> commands;
        client->handleIncomingData(buffer, valread, commands);
        this->handleBuffer(commands, client);
    }
}