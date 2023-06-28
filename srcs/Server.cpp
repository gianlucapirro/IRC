#include "Server.hpp"

Server::Server(const Config& config) : config(config) { setupServer(); }

void Server::setupServer() {
    int opt = 1;

    try {
        createSocket();
        setSocketOptions(opt);
        configureAddress();
        bindSocket();
        setListenMode();

        this->fds.resize(1);
        this->fds[0].fd = this->serverFD;
        this->fds[0].events = POLLIN;
        this->fds[0].revents = 0;
    } catch (const std::runtime_error& ex) {
        std::cerr << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::createSocket() {
    if ((this->serverFD = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        throw std::runtime_error("Server Error: Socket creation failed");
    }
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
    if (listen(this->serverFD, this->config.) < 0) {
        throw std::runtime_error("Server Error: Failed to set socket to listen mode");
    }
}

void Server::acceptNewConnection() {
    int new_socket;
    socklen_t addrlen = sizeof(this->address);
    if ((new_socket = accept(this->serverFD, (struct sockaddr*)&(this->address), &addrlen)) < 0) {
        std::runtime_error("Server Error: New connection not accepted");
        exit(EXIT_FAILURE);
    }

    pollfd new_fd;
    new_fd.fd = new_socket;
    new_fd.events = POLLIN;
    this->fds.push_back(new_fd);
    this->clients.push_back(Client(new_socket, this->config.getPassword()));
    std::cout << "Client Created" << std::endl;
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

void Server::processActivity(char* buffer) {
    for (size_t i = 0; i < this->fds.size(); i++) {
        if (this->fds[i].revents & POLLIN) {          // check if there is data to read in fd
            if (this->fds[i].fd == this->serverFD) {  // if there is new data to read on serverFD -> new client
                acceptNewConnection();
            } else {
                handleClient(i, buffer);  // else its from an existing client
            }
        }
    }
}

void Server::run() {
    try {
        while (true) {
            int activity = poll(&(this->fds[0]), this->fds.size(), -1);

            if (activity < 0) {
                std::runtime_error("Server Error: Error while polling");
                continue;
            }

            char buffer[BUFFER_SIZE] = {0};
            processActivity(buffer);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}
