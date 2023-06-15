#include "Server.hpp"

Server::Server(int port, std::string password) : port(port), password(password) {
    int opt = 1;

    try {
        if ((this->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            throw ServerException("Socket creation failed");
        }

        if (setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
            throw ServerException("Failed to attach socket to the port");
        }

        this->address.sin_family = AF_INET;          // ipv4
        this->address.sin_addr.s_addr = INADDR_ANY;  // any ip address
        this->address.sin_port = htons(this->port);  // port 6667

        // Bind the socket to address
        // By binding the server socket to a specific address, you ensure that
        // the server only accepts connections on that address and port
        // combination. Any incoming connection requests targeted at that
        // address and port will be directed to the server socket.
        if (bind(this->server_fd, (struct sockaddr*)&(this->address), sizeof(this->address)) < 0) {
            throw ServerException("Failed to bind");
        }

        // Listen is to set the server socket to the listening state, allowing
        // it to accept incoming connections. max_clients is the maximum amount of clients
        // that are allowed to be in qeue
        if (listen(this->server_fd, this->max_clients) < 0) {
            throw ServerException("Failed to set socket to listen mode");
        }

        this->fds.resize(1);
        this->fds[0].fd = this->server_fd;
        this->fds[0].events = POLLIN;
    } catch (const ServerException& ex) {
        std::cerr << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::run() {
    try {
        while (true) {
            // Poll checks for activity (new connections or clients)
            int activity = poll(&(this->fds[0]), this->fds.size(), -1);  // loops over all fds

            if (activity < 0) {
                Server::ServerException("Poll error");
                continue;
            }

            char buffer[1024] = {0};
            for (size_t i = 0; i < this->fds.size(); i++) {
                if (this->fds[i].revents & POLLIN) {
                    if (this->fds[i].fd == this->server_fd) {
                        // Accept new connection
                        int new_socket;
                        socklen_t addrlen = sizeof(this->address);
                        if ((new_socket = accept(this->server_fd, (struct sockaddr*)&(this->address), &addrlen)) < 0) {
                            Server::ServerException("New connection not accepted");
                            exit(EXIT_FAILURE);
                        }

                        std::cout << "New connection accepted" << std::endl;
                        pollfd new_fd;
                        new_fd.fd = new_socket;
                        new_fd.events = POLLIN;
                        this->fds.push_back(new_fd);
                    } else {
                        // Handle input from existing connection
                        ssize_t valread = read(this->fds[i].fd, buffer, sizeof(buffer));
                        if (valread <= 0) {
                            // Client disconnected, remove from the list
                            close(this->fds[i].fd);
                            this->fds.erase(this->fds.begin() + i);
                            i--;  // Adjust index because of erase
                        } else {
                            // Print received message
                            buffer[valread] = '\0';
                            std::cout << "Received: " << buffer << std::endl;
                        }
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}