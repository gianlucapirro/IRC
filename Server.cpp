#include "Server.hpp"

Server::Server(int port, std::string password) : port(port), password(password) { setupServer(); }

void Server::setupServer() {
    int opt = 1;

    try {
        createSocket();
        setSocketOptions(opt);
        configureAddress();
        bindSocket();
        setListenMode();

        this->fds.resize(1);
        this->fds[0].fd = this->server_fd;
        this->fds[0].events = POLLIN;
    } catch (const ServerException& ex) {
        std::cerr << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::createSocket() {
    if ((this->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        throw ServerException("Socket creation failed");
    }
}

void Server::setSocketOptions(int opt) {
    if (setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        throw ServerException("Failed to attach socket to the port");
    }
}

void Server::configureAddress() {
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(this->port);
}

void Server::bindSocket() {
    if (bind(this->server_fd, (struct sockaddr*)&(this->address), sizeof(this->address)) < 0) {
        throw ServerException("Failed to bind");
    }
}

void Server::setListenMode() {
    if (listen(this->server_fd, this->max_clients) < 0) {
        throw ServerException("Failed to set socket to listen mode");
    }
}

void Server::run() {
    try {
        while (true) {
            int activity = poll(&(this->fds[0]), this->fds.size(), -1);

            if (activity < 0) {
                Server::ServerException("Poll error");
                continue;
            }

            char buffer[1024] = {0};
            processActivity(buffer);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

void Server::processActivity(char* buffer) {
    for (size_t i = 0; i < this->fds.size(); i++) {
        if (this->fds[i].revents & POLLIN) {
            if (this->fds[i].fd == this->server_fd) {
                acceptNewConnection();
            } else {
                handleClient(i, buffer);
            }
        }
    }
}

void Server::acceptNewConnection() {
    int new_socket;
    socklen_t addrlen = sizeof(this->address);
    if ((new_socket = accept(this->server_fd, (struct sockaddr*)&(this->address), &addrlen)) < 0) {
        Server::ServerException("New connection not accepted");
        exit(EXIT_FAILURE);
    }

    pollfd new_fd;
    new_fd.fd = new_socket;
    new_fd.events = POLLIN;
    this->fds.push_back(new_fd);
    this->clients.push_back(Client(new_socket, this->password));
    std::cout << "Client Created" << std::endl;
}

void Server::handleClient(size_t i, char* buffer) {
    ssize_t valread = read(this->fds[i].fd, buffer, sizeof(buffer));
    if (valread <= 0) {
        close(this->fds[i].fd);
        this->fds.erase(this->fds.begin() + i);
        this->clients.erase(this->clients.begin() + i);
        i--;
    } else {
        buffer[valread] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    }
}
