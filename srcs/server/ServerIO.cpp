#include "Server.hpp"

void Server::sendMessage(int clientFD, const std::string& message) {
    send(clientFD, message.c_str(), message.length(), 0);
}

void Server::acceptNewConnection() {
    int new_socket;
    socklen_t addrlen = sizeof(this->address);
    if ((new_socket = accept(this->serverFD, (struct sockaddr*)&(this->address), &addrlen)) < 0) {
        throw std::runtime_error("Server Error: New connection not accepted");
        exit(EXIT_FAILURE);
    }

    std::pair<std::string, std::vector<std::string> > parsedCommand;
    char passwordBuffer[PASSWORD_BUFFER_SIZE];
    ssize_t valread = read(new_socket, passwordBuffer, PASSWORD_BUFFER_SIZE);
    if (valread > 0) {
        passwordBuffer[valread] = '\0';  // Null terminate the string
        parsedCommand = this->commandHandler.parseCommand(passwordBuffer);
        this->commandHandler.handleCommand(new_socket, parsedCommand.first, parsedCommand.second);
    }
}

void Server::handleClient(size_t i, char* buffer) {
    ssize_t valread = read(this->fds[i].fd, buffer, BUFFER_SIZE);
    if (valread <= 0) {
        close(this->fds[i].fd);
        this->fds.erase(this->fds.begin() + i);
        this->clients.erase(this->clients.begin() + i);
    } else {
        buffer[valread] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    }
}