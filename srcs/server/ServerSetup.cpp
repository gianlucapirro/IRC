#include "Server.hpp"

Server::Server(const Config& config) : config(config), commandHandler(&config, &(this->clients)) {
    setupServer();
}
void Server::setupServer() {
    int opt = 1;

    try {
        createSocket();
        setSocketOptions(opt);
        configureAddress();
        bindSocket();
        setListenMode();
        respondSetQueue(&this->messages);

        this->fds.resize(1);
        this->fds[0].fd = this->serverFD;
        this->fds[0].events = POLLIN;

        // Also create client for the server
        Client* newClient = new Client(this->serverFD);
        this->getClients().push_back(newClient);

    } catch (const std::runtime_error& ex) {
        std::cerr << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::createSocket() {
	this->serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverFD == 0) {
        throw std::runtime_error("Server Error: Socket creation failed");
    }
	setNonBlocking(this->serverFD);
}

void Server::setSocketOptions(int opt) {
    if (setsockopt(this->serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        throw std::runtime_error("Server Error: Failed to attach socket to the port");
    }
}

void Server::configureAddress() {
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(this->config.getPort());
}

void Server::bindSocket() {
    if (bind(this->serverFD, (struct sockaddr*)&(this->address), sizeof(this->address)) < 0) {
        throw std::runtime_error("Server Error: Failed to bind");
    }
}

void Server::setListenMode() {
    if (listen(this->serverFD, this->config.getMaxClients()) < 0) {
        throw std::runtime_error("Server Error: Failed to set socket to listen mode");
    }
}