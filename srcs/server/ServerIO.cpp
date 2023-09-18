#include "Server.hpp"
#include "utils.hpp"

void Server::sendMessage(int clientFD, const std::string& message) {
    // std::cout << message.length() << ": Message send: " << message << std::endl;
    send(clientFD, message.c_str(), message.length(), 0);
}

void Server::handleBuffer(std::vector<std::string> commands, Client* client) {
    std::vector<parsedCommand> parsedCommands = this->commandHandler.parseCommands(commands);
    for (std::vector<parsedCommand>::iterator it = parsedCommands.begin(); it != parsedCommands.end(); ++it) {
        int action = this->commandHandler.handleCommand(client, it->first, it->second);
        switch (action) {
            case QUIT_USER:
                this->commandHandler.channelHandler.removeClientFromAllChannels(client);
                client->del();
                return;
        }
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
    if (client->getDeleted() == true)
        return;
    char buffer[BUFFER_SIZE];
    ssize_t valread = read(this->fds[i].fd, buffer, BUFFER_SIZE);
	// std::cout << buffer << std::endl;

    if (valread <= 0) {
        this->commandHandler.channelHandler.removeClientFromAllChannels(client);
        client->del();
    } else {
        std::vector<std::string> commands;
        client->handleIncomingData(buffer, valread, commands);
        this->handleBuffer(commands, client);
    }
}


void Server::deleteClient(Client* client) {
    int clientIndex = -1;
    size_t i = 0;
    for (; i < this->clients.size(); i++) {
        if ((this->clients)[i]->getFD() == client->getFD()) {
            clientIndex = i;
            break;
        }
    }
    if (clientIndex == -1)
        throw std::runtime_error("Trying to delete a client that was no longer in the clients list");

    close((this->fds)[clientIndex].fd);
    this->fds.erase(this->fds.begin() + clientIndex);
    this->clients.erase(this->clients.begin() + clientIndex);
    

    delete client;
}