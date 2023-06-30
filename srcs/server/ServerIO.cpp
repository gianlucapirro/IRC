#include "Server.hpp"

void Server::sendMessage(int clientFD, const std::string& message) {
    // std::cout << message.length() << ": Message send: " << message << std::endl;
    send(clientFD, message.c_str(), message.length(), 0);
}

void Server::handleBuffer(char* buffer, int valread, int fd) {
    buffer[valread] = '\0';
    std::vector<std::pair<std::string, std::vector<std::string> > > parsedCommands =
        this->commandHandler.parseCommands(buffer);
    for (std::vector<std::pair<std::string, std::vector<std::string> > >::iterator it = parsedCommands.begin();
         it != parsedCommands.end(); ++it) {
        this->commandHandler.handleCommand(fd, it->first, it->second);
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
    Client& newClient = this->clients.back();  // assumes that createClient adds the new client at the end

    char tempBuffer[BUFFER_SIZE];
    ssize_t valread = read(newSocket, tempBuffer, BUFFER_SIZE);
    if (valread <= 0) {
        close(newSocket);
        this->fds.pop_back();
        this->clients.pop_back();
    } else {
        newClient.handleIncomingData(tempBuffer, valread, *this);
    }
}

void Server::handleClient(size_t i) {
    Client& client = this->clients[i];
    char buffer[BUFFER_SIZE];
    ssize_t valread = read(this->fds[i].fd, buffer, BUFFER_SIZE);
    if (valread <= 0) {
        close(this->fds[i].fd);
        this->fds.erase(this->fds.begin() + i);
        this->clients.erase(this->clients.begin() + i);
    } else {
        client.handleIncomingData(buffer, valread, *this);
    }
}