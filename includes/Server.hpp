#ifndef SERVER_HPP
#define SERVER_HPP

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include "Client.hpp"
#include "Config.hpp"

class Server {
   private:
    struct sockaddr_in address;   // family, address, port of server
    std::vector<pollfd> fds;      // stores all sockets fd
    std::vector<Client> clients;  // vector of connected clients
    int serverFD;                // socket fd of the server

    const Config config;

    void setupServer();
    void createSocket();
    void setSocketOptions(int opt);
    void configureAddress();
    void bindSocket();
    void setListenMode();
    void processActivity(char* buffer);
    void acceptNewConnection();
    void handleClient(size_t i, char* buffer);

   public:
    Server(const Config& config);
    void run();
};

#endif
